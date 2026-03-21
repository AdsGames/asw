#include "./asw/modules/sound.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <algorithm>
#include <array>
#include <cmath>

#include "./asw/modules/log.h"

namespace {
float master_volume = 1.0F;
float sfx_volume = 1.0F;
float music_volume = 1.0F;

std::array<MIX_Track*, 16> tracks;
MIX_Track* music_track = nullptr;
MIX_Mixer* mixer = nullptr;

float compute_sfx_volume(float vol)
{
    auto volume = vol * sfx_volume;
    return std::clamp(volume, 0.0F, 1.0F);
}

float compute_music_volume(float vol)
{
    auto volume = vol * music_volume;
    return std::clamp(volume, 0.0F, 1.0F);
}

int find_free_track()
{
    for (size_t i = 0; i < tracks.size(); ++i) {
        if (!MIX_TrackPlaying(tracks[i])) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace

MIX_Mixer* asw::sound::get_mixer()
{
    return mixer;
}

void asw::sound::_shutdown()
{
    auto* m = mixer;
    mixer = nullptr;
    if (m != nullptr) {
        MIX_DestroyMixer(m);
    }
    MIX_Quit();
}

bool asw::sound::_init()
{
    if (!MIX_Init()) {
        asw::log::error("Failed to initialize SDL_mixer: {}", SDL_GetError());
        return false;
    }

    if (mixer != nullptr) {
        asw::log::warn("Mixer already initialized");
        return true;
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

    for (auto& track : tracks) {
        track = MIX_CreateTrack(mixer);
        if (track == nullptr) {
            asw::log::error("Failed to create track: {}", SDL_GetError());
            return false;
        }
    }

    music_track = MIX_CreateTrack(mixer);
    if (music_track == nullptr) {
        asw::log::error("Failed to create music track: {}", SDL_GetError());
        return false;
    }

    return true;
}

void asw::sound::play(const asw::Sample& sample, float volume, float pan, bool loop)
{
    const int channel = find_free_track();
    if (channel >= 0) {
        // Find first free track and play sample on it
        auto& track = tracks[channel];
        MIX_SetTrackAudio(track, sample.get());
        MIX_SetTrackGain(track, compute_sfx_volume(volume));

        // Stereo gains for panning using equal power panning
        const float left = std::sqrtf((1.0F - pan) * 0.5F);
        const float right = std::sqrtf((1.0F + pan) * 0.5F);

        MIX_StereoGains gains;
        gains.left = left;
        gains.right = right;
        MIX_SetTrackStereo(track, &gains);

        // Play the track, looping if requested
        const SDL_PropertiesID options = SDL_CreateProperties();
        SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0);
        MIX_PlayTrack(track, options);
    }
}

void asw::sound::play_music(const asw::Music& sample, float volume, float fade_in_s)
{
    MIX_SetTrackGain(music_track, compute_music_volume(volume));
    MIX_SetTrackAudio(music_track, sample.get());

    const SDL_PropertiesID options = SDL_CreateProperties();
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    SDL_SetNumberProperty(
        options, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, static_cast<int>(fade_in_s * 1000.0F));
    MIX_PlayTrack(music_track, options);
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
    sfx_volume = std::clamp(volume, 0.0F, 1.0F);
}

void asw::sound::set_music_volume(float volume)
{
    music_volume = std::clamp(volume, 0.0F, 1.0F);
}

float asw::sound::get_master_volume()
{
    return master_volume;
}

float asw::sound::get_sfx_volume()
{
    return sfx_volume;
}

float asw::sound::get_music_volume()
{
    return music_volume;
}
