/// @file sound.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Sound module for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_SOUND_H
#define ASW_SOUND_H

#include <cstdint>

#include "./geometry.h"
#include "./types.h"

namespace asw::sound {

/// @brief Mix buses. Each sound plays on one bus, and each bus has its own
/// volume and can be ducked.
///
enum class Bus : uint8_t {
    Sfx,
    Music,
    Ambient,
    Ui,
    Voice,
    Count,
};

/// @brief How volume falls off with distance for positional sounds.
///
enum class Rolloff : uint8_t {
    /// Straight line from full volume at min_distance to silent at max_distance.
    Linear,
    /// Natural sounding, loud close up and a long quiet tail.
    Inverse,
    /// Like Inverse but drops faster.
    InverseSquare,
};

/// @brief How positional sounds are placed on the speakers.
///
enum class SpatialMode : uint8_t {
    /// Left/right panning. Best for headphones and side on games.
    Stereo,
    /// SDL_mixer 3D positioning, also uses rear and side speakers. The world
    /// is treated as top down: up the screen is in front, down is behind.
    Surround,
};

/// @brief Distance settings for positional sounds, in world units (pixels).
///
struct Attenuation {
    /// Full volume at this distance or closer.
    float min_distance { 100.0F };

    /// Silent at this distance or further.
    float max_distance { 1000.0F };

    /// Shape of the fall off between the two.
    Rolloff rolloff { Rolloff::Inverse };
};

/// @brief Options for playing a sound.
///
struct PlayOptions {
    /// Playback volume (0.0 - 1.0).
    float volume { 1.0F };

    /// Panning (-1.0 left - 1.0 right). Ignored for positional sounds.
    float pan { 0.0F };

    /// Loop forever.
    bool loop { false };

    /// Playback speed and pitch. 1.0 is normal, 2.0 is an octave up.
    float pitch { 1.0F };

    /// Random pitch change per play, as a fraction. 0.05 picks between 0.95x
    /// and 1.05x. Stops repeated sounds (footsteps, hits) sounding robotic.
    float pitch_variation { 0.0F };

    /// Random volume change per play, as a fraction.
    float volume_variation { 0.0F };

    /// Bus to play on.
    Bus bus { Bus::Sfx };

    /// When every voice is busy, a new sound replaces the quietest voice with
    /// the same or lower priority. If there is none, the new sound is dropped.
    int priority { 0 };

    /// Fade in time in seconds.
    float fade_in_s { 0.0F };

    /// Distance fall off, used by positional sounds.
    Attenuation attenuation {};
};

struct HandleAccess;

/// @brief A handle to a playing sound, used to change it while it plays.
///
/// @details Handles are cheap to copy. Once the sound ends or its voice is
/// given to another sound, the handle goes stale and every call on it does
/// nothing.
///
class SoundHandle {
public:
    /// @brief Create a handle that refers to nothing.
    ///
    SoundHandle() = default;

    /// @brief Check if the sound is still playing or paused.
    ///
    /// @return True while the sound has not ended.
    ///
    bool is_playing() const;

    /// @brief Stop the sound.
    ///
    /// @param fade_out_s Fade out time in seconds.
    ///
    void stop(float fade_out_s = 0.0F) const;

    /// @brief Pause the sound.
    ///
    void pause() const;

    /// @brief Resume a paused sound.
    ///
    void resume() const;

    /// @brief Set the volume.
    ///
    /// @param volume Volume (0.0 - 1.0), before bus and distance.
    ///
    void set_volume(float volume) const;

    /// @brief Set the playback speed and pitch.
    ///
    /// @param pitch 1.0 is normal.
    ///
    void set_pitch(float pitch) const;

    /// @brief Set the panning. Makes a positional sound non positional.
    ///
    /// @param pan Panning (-1.0 left - 1.0 right).
    ///
    void set_pan(float pan) const;

    /// @brief Set where the sound is in the world. Makes the sound positional.
    ///
    /// @param position The world position.
    ///
    void set_position(const Vec2<float>& position) const;

    /// @brief Set how fast the sound is moving, for doppler pitch shift.
    ///
    /// @param velocity Velocity in world units per second.
    ///
    void set_velocity(const Vec2<float>& velocity) const;

    /// @brief Muffle the sound, as if heard through a wall.
    ///
    /// @param amount 0.0 is clear, 1.0 is fully muffled.
    ///
    void set_occlusion(float amount) const;

    /// @brief Change the distance fall off.
    ///
    /// @param attenuation The new settings.
    ///
    void set_attenuation(const Attenuation& attenuation) const;

private:
    friend struct HandleAccess;

    SoundHandle(int voice, uint32_t generation);

    int voice { -1 };
    uint32_t generation { 0 };
};

/// @brief Initialize the sound module. Called automatically by asw::core::init().
///
/// @return True if initialization was successful, false otherwise.
///
bool _init();

/// @brief Update fades, ducking and positional sounds. Called automatically by
/// asw::core::update().
///
void _update();

/// @brief Shut down the sound module. Called automatically by asw::core::shutdown().
///
void _shutdown();

/// @brief Get the SDL mixer device.
///
/// @return Pointer to the MIX_Mixer, or nullptr if not initialized.
///
MIX_Mixer* get_mixer();

/// @brief Play a sample.
///
/// @param sample Sample to play
/// @param volume Playback volume (0.0 - 1.0).
/// @param pan Panning (-1.0 - 1.0), where -1.0 is full left, 0.0 is center,
/// and 1.0 is full right.
/// @param loop Whether to loop the sample (false = no loop, true = infinite
/// loop).
/// @return A handle to the sound, stale if it could not be played.
///
SoundHandle play(
    const asw::Sample& sample, float volume = 1.0F, float pan = 0.0F, bool loop = false);

/// @brief Play a sample with options.
///
/// @param sample Sample to play
/// @param options How to play it.
/// @return A handle to the sound, stale if it could not be played.
///
SoundHandle play(const asw::Sample& sample, const PlayOptions& options);

/// @brief Play a sample at a point in the world. It is panned, faded and
/// pitch shifted relative to the listener, and keeps updating while it plays.
///
/// @param sample Sample to play
/// @param position The world position.
/// @param options How to play it.
/// @return A handle to the sound, stale if it could not be played. One shot
/// sounds out of hearing range are not played.
///
SoundHandle play_positional(
    const asw::Sample& sample, const Vec2<float>& position, const PlayOptions& options = {});

/// @brief Play a sample panned by where it happens on screen. Sounds pan
/// towards the side they are on and fade out once they are off screen, going
/// silent a full screen width past the edge.
///
/// @param sample Sample to play
/// @param x Horizontal position in logical screen coordinates.
/// @param volume Playback volume (0.0 - 1.0) before distance fading.
/// @return A handle to the sound, stale if it could not be played.
///
SoundHandle play_at(const asw::Sample& sample, float x, float volume = 1.0F);

/// @brief Play a sample panned by where it happens on screen, with options.
/// Works like play_at above, so pitch, variation, bus and priority can be set
/// too. The pan is worked out once when the sound starts.
///
/// @param sample Sample to play
/// @param x Horizontal position in logical screen coordinates.
/// @param options How to play it. volume is before distance fading, pan is
/// replaced by the screen position.
/// @return A handle to the sound, stale if it could not be played.
///
SoundHandle play_at(const asw::Sample& sample, float x, const PlayOptions& options);

/// @brief Set where the listener is, usually the player or camera centre.
///
/// @param position The world position.
/// @param velocity Velocity in world units per second, for doppler.
///
void set_listener(const Vec2<float>& position, const Vec2<float>& velocity = { 0.0F, 0.0F });

/// @brief Get the listener position.
///
/// @return The world position.
///
Vec2<float> get_listener();

/// @brief Choose how positional sounds are placed on the speakers.
///
/// @param mode The mode. Defaults to Stereo.
///
void set_spatial_mode(SpatialMode mode);

/// @brief Set how strong doppler pitch shift is.
///
/// @param factor 0 turns doppler off, 1 is realistic. Defaults to 1.
/// @param speed_of_sound World units per second. Lower makes the effect
/// stronger. Defaults to 3000.
///
void set_doppler(float factor, float speed_of_sound = 3000.0F);

/// @brief Set a bus volume.
///
/// @param bus The bus.
/// @param volume Volume multiplier (0.0 - 1.0).
///
void set_bus_volume(Bus bus, float volume);

/// @brief Get a bus volume.
///
/// @param bus The bus.
/// @return The volume multiplier (0.0 - 1.0).
///
float get_bus_volume(Bus bus);

/// @brief Turn a bus down for a while, for example music under dialogue or
/// everything under a big explosion. Overlapping ducks keep the deepest level
/// and the longest hold.
///
/// @param bus The bus to turn down.
/// @param gain Level while ducked (0.0 - 1.0).
/// @param hold_s Seconds to stay ducked.
/// @param fade_s Seconds to fade down and back up.
///
void duck(Bus bus, float gain, float hold_s, float fade_s = 0.2F);

/// @brief Play a music sample.
///
/// @param sample Sample to play
/// @param volume Playback volume (0.0 - 1.0).
/// @param fade_in_s Fade-in duration in seconds.
///
void play_music(const asw::Music& sample, float volume = 1.0F, float fade_in_s = 0.0F);

/// @brief Stop the currently playing music.
///
void stop_music(float fade_out_s = 0.0F);

/// @brief Pause the currently playing music.
///
void pause_music();

/// @brief Resume paused music.
///
void resume_music();

/// @brief Check if music is currently playing.
///
/// @return True if music is playing.
///
bool is_music_playing();

/// @brief Check if music is paused.
///
/// @return True if music is paused.
///
bool is_music_paused();

/// @brief Set the master volume multiplier (affects all audio).
///
/// @param volume Volume multiplier (0.0 - 1.0).
///
void set_master_volume(float volume);

/// @brief Set the SFX volume multiplier. Same as the Sfx bus volume.
///
/// @param volume Volume multiplier (0.0 - 1.0).
///
void set_sfx_volume(float volume);

/// @brief Set the music volume multiplier. Same as the Music bus volume.
///
/// @param volume Volume multiplier (0.0 - 1.0).
///
void set_music_volume(float volume);

/// @brief Get the current master volume multiplier.
///
/// @return The master volume (0.0 - 1.0).
///
float get_master_volume();

/// @brief Get the current SFX volume multiplier.
///
/// @return The SFX volume (0.0 - 1.0).
///
float get_sfx_volume();

/// @brief Get the current music volume multiplier.
///
/// @return The music volume (0.0 - 1.0).
///
float get_music_volume();

} // namespace asw::sound

#endif // ASW_SOUND_H
