#include "./asw/modules/sound.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

#include "./asw/modules/display.h"
#include "./asw/modules/log.h"
#include "./asw/modules/random.h"

namespace {
constexpr size_t NUM_VOICES = 32;
constexpr size_t NUM_BUSES = static_cast<size_t>(asw::sound::Bus::Count);
constexpr int MAX_FILTER_CHANNELS = 8;

// Cutoff at full occlusion, and the level it takes the volume down to
constexpr float OCCLUDED_CUTOFF_HZ = 500.0F;
constexpr float OPEN_CUTOFF_HZ = 20000.0F;
constexpr float OCCLUDED_GAIN = 0.5F;

// Keep doppler musical, even for very fast sources
constexpr float MIN_DOPPLER = 0.5F;
constexpr float MAX_DOPPLER = 2.0F;

// One pole low pass run on the audio thread. The game thread only writes the
// cutoff, the audio thread owns the filter state.
struct LowPass {
    std::atomic<float> cutoff_hz { OPEN_CUTOFF_HZ };
    std::atomic<bool> reset { true };
    std::array<float, MAX_FILTER_CHANNELS> state {};
};

struct Voice {
    MIX_Track* track = nullptr;
    uint32_t generation = 0;

    bool positional = false;
    asw::Vec2<float> position;
    asw::Vec2<float> velocity;
    asw::sound::Attenuation attenuation;

    float volume = 1.0F;
    float pan = 0.0F;
    float pitch = 1.0F;
    float occlusion = 0.0F;
    asw::sound::Bus bus = asw::sound::Bus::Sfx;
    int priority = 0;

    // Gain after bus, distance and occlusion, used to pick a voice to steal
    float effective_gain = 0.0F;

    // What was last sent to the track. Each SDL_mixer call takes the mixer
    // lock, so unchanged values are not sent again.
    struct Sent {
        enum class Placement { None, Stereo, Point };
        Placement placement = Placement::None;
        float a = 0.0F;
        float b = 0.0F;
        float gain = -1.0F;
        float ratio = -1.0F;
    } sent;

    LowPass filter;
};

struct Duck {
    float gain = 1.0F;
    float target = 1.0F;
    float hold_s = 0.0F;
    float fade_s = 0.2F;
};

float master_volume = 1.0F;
std::array<float, NUM_BUSES> bus_volume = { 1.0F, 1.0F, 1.0F, 1.0F, 1.0F };
std::array<Duck, NUM_BUSES> ducks {};

std::array<Voice, NUM_VOICES> voices;
MIX_Track* music_track = nullptr;
float music_base_volume = 1.0F;
float music_sent_gain = -1.0F;
MIX_Mixer* mixer = nullptr;

// Counts shutdowns, each one frees all loaded audio
uint32_t session = 0;

asw::Vec2<float> listener_position;
asw::Vec2<float> listener_velocity;
asw::sound::SpatialMode spatial_mode = asw::sound::SpatialMode::Stereo;
float doppler_factor = 1.0F;
float speed_of_sound = 3000.0F;

uint64_t last_update_ns = 0;

float bus_gain(asw::sound::Bus bus)
{
    const auto i = static_cast<size_t>(bus);
    return bus_volume[i] * ducks[i].gain;
}

bool is_active(const Voice& v)
{
    return v.track != nullptr && (MIX_TrackPlaying(v.track) || MIX_TrackPaused(v.track));
}

void SDLCALL low_pass_callback(
    void* userdata, MIX_Track* /*track*/, const SDL_AudioSpec* spec, float* pcm, int samples)
{
    auto* filter = static_cast<LowPass*>(userdata);
    const int channels = spec->channels;
    if (channels <= 0 || channels > MAX_FILTER_CHANNELS || samples < channels) {
        return;
    }

    auto& state = filter->state;
    if (filter->reset.exchange(false)) {
        state.fill(0.0F);
    }

    const int frames = samples / channels;
    const float cutoff = filter->cutoff_hz.load(std::memory_order_relaxed);

    // Pass through, but keep the state on the signal so turning the filter
    // on later does not click
    if (cutoff >= OPEN_CUTOFF_HZ) {
        for (int c = 0; c < channels; ++c) {
            state[c] = pcm[((frames - 1) * channels) + c];
        }
        return;
    }

    const float alpha = 1.0F
        - std::exp((-2.0F * SDL_PI_F * cutoff) / static_cast<float>(std::max(1, spec->freq)));

    for (int f = 0; f < frames; ++f) {
        for (int c = 0; c < channels; ++c) {
            float& sample = pcm[(f * channels) + c];
            state[c] += alpha * (sample - state[c]);
            sample = state[c];
        }
    }
}

float attenuate(const asw::sound::Attenuation& att, float distance)
{
    const float near = std::max(att.min_distance, 0.001F);
    const float far = std::max(att.max_distance, near + 0.001F);

    if (distance <= near) {
        return 1.0F;
    }
    if (distance >= far) {
        return 0.0F;
    }

    // Inverse curves are shifted and scaled to hit exactly zero at far, so
    // sounds fade out instead of cutting off
    switch (att.rolloff) {
    case asw::sound::Rolloff::Linear:
        return 1.0F - ((distance - near) / (far - near));

    case asw::sound::Rolloff::Inverse: {
        const float floor = near / far;
        return ((near / distance) - floor) / (1.0F - floor);
    }

    case asw::sound::Rolloff::InverseSquare: {
        const float floor = (near * near) / (far * far);
        return (((near * near) / (distance * distance)) - floor) / (1.0F - floor);
    }
    }

    return 1.0F;
}

float doppler(const Voice& v)
{
    if (doppler_factor <= 0.0F || speed_of_sound <= 0.0F) {
        return 1.0F;
    }

    // Same model as OpenAL, along the line from source to listener
    const auto to_listener = listener_position - v.position;
    const float distance = to_listener.magnitude();
    if (distance < 0.001F) {
        return 1.0F;
    }

    const auto dir = to_listener / distance;
    const float limit = speed_of_sound * 0.99F;
    const float listener_speed
        = std::clamp(listener_velocity.dot(dir) * doppler_factor, -limit, limit);
    const float source_speed = std::clamp(v.velocity.dot(dir) * doppler_factor, -limit, limit);

    const float ratio = (speed_of_sound - listener_speed) / (speed_of_sound - source_speed);
    return std::clamp(ratio, MIN_DOPPLER, MAX_DOPPLER);
}

void set_stereo_pan(Voice& v, float pan)
{
    pan = std::clamp(pan, -1.0F, 1.0F);
    if (v.sent.placement == Voice::Sent::Placement::Stereo && v.sent.a == pan) {
        return;
    }
    v.sent = { .placement = Voice::Sent::Placement::Stereo,
        .a = pan,
        .gain = v.sent.gain,
        .ratio = v.sent.ratio };

    // Equal power panning
    MIX_StereoGains gains;
    gains.left = std::sqrt((1.0F - pan) * 0.5F);
    gains.right = std::sqrt((1.0F + pan) * 0.5F);
    MIX_SetTrackStereo(v.track, &gains);
}

void set_point(Voice& v, float x, float z)
{
    if (v.sent.placement == Voice::Sent::Placement::Point && v.sent.a == x && v.sent.b == z) {
        return;
    }
    v.sent = { .placement = Voice::Sent::Placement::Point,
        .a = x,
        .b = z,
        .gain = v.sent.gain,
        .ratio = v.sent.ratio };

    const MIX_Point3D point { x, 0.0F, z };
    MIX_SetTrack3DPosition(v.track, &point);
}

// Push a voice's state to its track
void apply(Voice& v)
{
    float gain = v.volume * bus_gain(v.bus);
    float ratio = v.pitch;

    if (v.positional) {
        const auto delta = v.position - listener_position;
        const float distance = delta.magnitude();
        gain *= attenuate(v.attenuation, distance);
        ratio *= doppler(v);

        if (spatial_mode == asw::sound::SpatialMode::Surround) {
            // Unit length, so SDL_mixer only picks the direction and our
            // attenuation sets the volume. Down the screen is behind.
            if (distance > 0.001F) {
                set_point(v, delta.x / distance, delta.y / distance);
            } else {
                set_point(v, 0.0F, 0.0F);
            }
        } else {
            // Sine of the angle to the listener, centred inside min_distance.
            // Kept off the extremes so the far ear still hears a little.
            const float spread = std::max(distance, v.attenuation.min_distance);
            set_stereo_pan(v, (delta.x / std::max(spread, 0.001F)) * 0.8F);
        }
    } else {
        set_stereo_pan(v, v.pan);
    }

    gain *= 1.0F - ((1.0F - OCCLUDED_GAIN) * v.occlusion);
    v.filter.cutoff_hz.store(v.occlusion > 0.0F
            ? OPEN_CUTOFF_HZ * std::pow(OCCLUDED_CUTOFF_HZ / OPEN_CUTOFF_HZ, v.occlusion)
            : OPEN_CUTOFF_HZ,
        std::memory_order_relaxed);

    v.effective_gain = std::clamp(gain, 0.0F, 1.0F);
    if (v.sent.gain != v.effective_gain) {
        v.sent.gain = v.effective_gain;
        MIX_SetTrackGain(v.track, v.effective_gain);
    }

    ratio = std::clamp(ratio, 0.01F, 100.0F);
    if (v.sent.ratio != ratio) {
        v.sent.ratio = ratio;
        MIX_SetTrackFrequencyRatio(v.track, ratio);
    }
}

void apply_music()
{
    const float gain = std::clamp(music_base_volume * bus_gain(asw::sound::Bus::Music), 0.0F, 1.0F);
    if (music_sent_gain != gain) {
        music_sent_gain = gain;
        MIX_SetTrackGain(music_track, gain);
    }
}

// Free voice, or the quietest voice this priority may replace
int find_voice(int priority)
{
    int best = -1;
    for (size_t i = 0; i < voices.size(); ++i) {
        const auto& v = voices[i];
        if (v.track == nullptr) {
            continue;
        }
        if (!is_active(v)) {
            return static_cast<int>(i);
        }
        if (v.priority > priority) {
            continue;
        }

        if (best < 0 || v.priority < voices[best].priority
            || (v.priority == voices[best].priority
                && v.effective_gain < voices[best].effective_gain)) {
            best = static_cast<int>(i);
        }
    }
    return best;
}

float vary(float value, float variation)
{
    if (variation <= 0.0F) {
        return value;
    }
    return value * (1.0F + asw::random::between(-variation, variation));
}

} // namespace

// ---- SoundHandle ----
//

asw::sound::SoundHandle::SoundHandle(int voice, uint32_t generation)
    : voice(voice)
    , generation(generation)
{
}

namespace {
// Voice the handle points at, or nullptr if the handle is stale
Voice* resolve(int voice, uint32_t generation)
{
    if (voice < 0 || static_cast<size_t>(voice) >= voices.size()) {
        return nullptr;
    }

    auto& v = voices[voice];
    if (v.generation != generation || !is_active(v)) {
        return nullptr;
    }
    return &v;
}
} // namespace

bool asw::sound::SoundHandle::is_playing() const
{
    return resolve(voice, generation) != nullptr;
}

void asw::sound::SoundHandle::stop(float fade_out_s) const
{
    if (auto* v = resolve(voice, generation)) {
        MIX_StopTrack(v->track,
            MIX_TrackMSToFrames(v->track, static_cast<Sint64>(fade_out_s * 1000.0F)));
    }
}

void asw::sound::SoundHandle::pause() const
{
    if (auto* v = resolve(voice, generation)) {
        MIX_PauseTrack(v->track);
    }
}

void asw::sound::SoundHandle::resume() const
{
    if (auto* v = resolve(voice, generation)) {
        MIX_ResumeTrack(v->track);
    }
}

void asw::sound::SoundHandle::set_volume(float volume) const
{
    if (auto* v = resolve(voice, generation)) {
        v->volume = std::clamp(volume, 0.0F, 1.0F);
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_pitch(float pitch) const
{
    if (auto* v = resolve(voice, generation)) {
        v->pitch = pitch;
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_pan(float pan) const
{
    if (auto* v = resolve(voice, generation)) {
        v->positional = false;
        v->pan = pan;
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_position(const Vec2<float>& position) const
{
    if (auto* v = resolve(voice, generation)) {
        v->positional = true;
        v->position = position;
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_velocity(const Vec2<float>& velocity) const
{
    if (auto* v = resolve(voice, generation)) {
        v->velocity = velocity;
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_occlusion(float amount) const
{
    if (auto* v = resolve(voice, generation)) {
        v->occlusion = std::clamp(amount, 0.0F, 1.0F);
        apply(*v);
    }
}

void asw::sound::SoundHandle::set_attenuation(const Attenuation& attenuation) const
{
    if (auto* v = resolve(voice, generation)) {
        v->attenuation = attenuation;
        apply(*v);
    }
}

// ---- Module ----
//

MIX_Mixer* asw::sound::get_mixer()
{
    return mixer;
}

void asw::sound::_shutdown()
{
    auto* m = mixer;
    mixer = nullptr;
    if (m != nullptr) {
        // Destroying the mixer destroys its tracks
        MIX_DestroyMixer(m);
    }
    for (auto& v : voices) {
        v.track = nullptr;
    }
    music_track = nullptr;
    MIX_Quit();
    session++;
}

uint32_t asw::sound::_get_session()
{
    return session;
}

bool asw::sound::_init()
{
    // Before MIX_Init, so the init count matches the one MIX_Quit in _shutdown
    if (mixer != nullptr) {
        asw::log::warn("Mixer already initialized");
        return true;
    }

    if (!MIX_Init()) {
        asw::log::error("Failed to initialize SDL_mixer: {}", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16LE;
    spec.freq = 44100;
    spec.channels = 2;

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (mixer == nullptr) {
        asw::log::error("Failed to create mixer: {}", SDL_GetError());
        return false;
    }

    for (auto& v : voices) {
        v.track = MIX_CreateTrack(mixer);
        v.sent = { };
        if (v.track == nullptr) {
            asw::log::error("Failed to create track: {}", SDL_GetError());
            return false;
        }
        MIX_SetTrackCookedCallback(v.track, low_pass_callback, &v.filter);
    }

    music_track = MIX_CreateTrack(mixer);
    music_sent_gain = -1.0F;
    if (music_track == nullptr) {
        asw::log::error("Failed to create music track: {}", SDL_GetError());
        return false;
    }

    last_update_ns = SDL_GetTicksNS();
    return true;
}

void asw::sound::_update()
{
    if (mixer == nullptr) {
        return;
    }

    const uint64_t now = SDL_GetTicksNS();
    const float dt = static_cast<float>(now - last_update_ns) / 1'000'000'000.0F;
    last_update_ns = now;

    // Ducking: fade towards the target while held, then back to full
    for (auto& d : ducks) {
        float target = 1.0F;
        if (d.hold_s > 0.0F) {
            d.hold_s -= dt;
            target = d.target;
        } else {
            d.target = 1.0F;
        }

        const float step = dt / std::max(d.fade_s, 0.001F);
        d.gain = d.gain < target ? std::min(d.gain + step, target) : std::max(d.gain - step, target);
    }

    for (auto& v : voices) {
        if (is_active(v)) {
            apply(v);
        }
    }

    apply_music();
}

// Creates handles, which only the sound module may do
struct asw::sound::HandleAccess {
    static SoundHandle make(int voice, uint32_t generation) { return { voice, generation }; }
};

namespace {
// Start a sample on a voice. Position is set before playback starts, so a
// positional sound never plays a few samples unpanned at full volume.
asw::sound::SoundHandle start(const asw::Sample& sample, const asw::sound::PlayOptions& options,
    const asw::Vec2<float>* position)
{
    if (mixer == nullptr || sample == nullptr || static_cast<size_t>(options.bus) >= NUM_BUSES) {
        return {};
    }

    const int index = find_voice(options.priority);
    if (index < 0) {
        return {};
    }

    auto& v = voices[index];
    if (is_active(v)) {
        MIX_StopTrack(v.track, 0);
    }

    v.generation++;
    v.positional = position != nullptr;
    v.position = position != nullptr ? *position : asw::Vec2<float>(0.0F, 0.0F);
    v.velocity = asw::Vec2<float>(0.0F, 0.0F);
    v.attenuation = options.attenuation;
    v.volume = std::clamp(vary(options.volume, options.volume_variation), 0.0F, 1.0F);
    v.pan = options.pan;
    v.pitch = vary(options.pitch, options.pitch_variation);
    v.occlusion = 0.0F;
    v.bus = options.bus;
    v.priority = options.priority;
    v.filter.reset.store(true);

    // Send everything for the new sound
    v.sent = { };
    MIX_SetTrackAudio(v.track, sample.get());
    apply(v);

    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, options.loop ? -1 : 0);
    if (options.fade_in_s > 0.0F) {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER,
            static_cast<Sint64>(options.fade_in_s * 1000.0F));
    }
    const bool played = MIX_PlayTrack(v.track, props);
    SDL_DestroyProperties(props);

    if (!played) {
        return {};
    }

    return asw::sound::HandleAccess::make(index, v.generation);
}
} // namespace

asw::sound::SoundHandle asw::sound::play(const asw::Sample& sample, const PlayOptions& options)
{
    return start(sample, options, nullptr);
}

asw::sound::SoundHandle asw::sound::play(
    const asw::Sample& sample, float volume, float pan, bool loop)
{
    PlayOptions options;
    options.volume = volume;
    options.pan = pan;
    options.loop = loop;
    return play(sample, options);
}

asw::sound::SoundHandle asw::sound::play_positional(
    const asw::Sample& sample, const Vec2<float>& position, const PlayOptions& options)
{
    // Skip one shots nobody can hear, so they do not take a voice
    const float distance = (position - listener_position).magnitude();
    if (!options.loop && attenuate(options.attenuation, distance) <= 0.0F) {
        return {};
    }

    return start(sample, options, &position);
}

asw::sound::SoundHandle asw::sound::play_at(const asw::Sample& sample, float x, float volume)
{
    PlayOptions options;
    options.volume = volume;
    return play_at(sample, x, options);
}

asw::sound::SoundHandle asw::sound::play_at(
    const asw::Sample& sample, float x, const PlayOptions& options)
{
    const auto width = static_cast<float>(asw::display::get_logical_size().x);
    if (width <= 0.0F) {
        return play(sample, options);
    }

    // -1 at the left edge, 1 at the right edge
    const float center = width / 2.0F;
    const float offset = (x - center) / center;

    // Full volume on screen, silent a screen width past either edge. The
    // edges are at +-1, so a screen width past them is +-3
    const float falloff = std::clamp((3.0F - std::abs(offset)) / 2.0F, 0.0F, 1.0F);
    if (falloff <= 0.0F) {
        return {};
    }

    PlayOptions panned = options;
    panned.volume = std::clamp(options.volume * falloff, 0.0F, 1.0F);
    panned.pan = std::clamp(offset * 0.7F, -1.0F, 1.0F);
    return play(sample, panned);
}

void asw::sound::set_listener(const Vec2<float>& position, const Vec2<float>& velocity)
{
    listener_position = position;
    listener_velocity = velocity;
}

asw::Vec2<float> asw::sound::get_listener()
{
    return listener_position;
}

void asw::sound::set_spatial_mode(SpatialMode mode)
{
    spatial_mode = mode;
}

void asw::sound::set_doppler(float factor, float speed)
{
    doppler_factor = std::max(0.0F, factor);
    speed_of_sound = std::max(1.0F, speed);
}

void asw::sound::set_bus_volume(Bus bus, float volume)
{
    if (bus == Bus::Count) {
        return;
    }
    bus_volume[static_cast<size_t>(bus)] = std::clamp(volume, 0.0F, 1.0F);
}

float asw::sound::get_bus_volume(Bus bus)
{
    if (bus == Bus::Count) {
        return 0.0F;
    }
    return bus_volume[static_cast<size_t>(bus)];
}

void asw::sound::duck(Bus bus, float gain, float hold_s, float fade_s)
{
    if (bus == Bus::Count) {
        return;
    }

    auto& d = ducks[static_cast<size_t>(bus)];
    d.target = std::min(d.hold_s > 0.0F ? d.target : 1.0F, std::clamp(gain, 0.0F, 1.0F));
    d.hold_s = std::max(d.hold_s, hold_s);
    d.fade_s = std::max(0.0F, fade_s);
}

void asw::sound::play_music(const asw::Music& sample, float volume, float fade_in_s)
{
    music_base_volume = std::clamp(volume, 0.0F, 1.0F);
    apply_music();
    MIX_SetTrackAudio(music_track, sample.get());

    const SDL_PropertiesID options = SDL_CreateProperties();
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    SDL_SetNumberProperty(
        options, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, static_cast<int>(fade_in_s * 1000.0F));
    MIX_PlayTrack(music_track, options);
    SDL_DestroyProperties(options);
}

void asw::sound::stop_music(float fade_out_s)

{
    const auto fade_out_frames
        = MIX_TrackMSToFrames(music_track, static_cast<Sint64>(fade_out_s * 1000.0F));
    MIX_StopTrack(music_track, fade_out_frames);
}

void asw::sound::pause_music()
{
    MIX_PauseTrack(music_track);
}

void asw::sound::resume_music()
{
    MIX_ResumeTrack(music_track);
}

bool asw::sound::is_music_playing()
{
    return MIX_TrackPlaying(music_track);
}

bool asw::sound::is_music_paused()
{
    return MIX_TrackPaused(music_track);
}

void asw::sound::set_master_volume(float volume)
{
    master_volume = std::clamp(volume, 0.0F, 1.0F);
    MIX_SetMixerGain(mixer, master_volume);
}

void asw::sound::set_sfx_volume(float volume)
{
    set_bus_volume(Bus::Sfx, volume);
}

void asw::sound::set_music_volume(float volume)
{
    set_bus_volume(Bus::Music, volume);
    apply_music();
}

float asw::sound::get_master_volume()
{
    return master_volume;
}

float asw::sound::get_sfx_volume()
{
    return get_bus_volume(Bus::Sfx);
}

float asw::sound::get_music_volume()
{
    return get_bus_volume(Bus::Music);
}
