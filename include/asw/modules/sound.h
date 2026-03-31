/// @file sound.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Sound module for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_SOUND_H
#define ASW_SOUND_H

#include "./types.h"

namespace asw::sound {

/// @brief Initialize the sound module. Called automatically by asw::core::init().
///
/// @return True if initialization was successful, false otherwise.
///
bool _init();

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
///
void play(const asw::Sample& sample, float volume = 1.0F, float pan = 0.5F, bool loop = false);

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

/// @brief Set the SFX volume multiplier.
///
/// @param volume Volume multiplier (0.0 - 1.0).
///
void set_sfx_volume(float volume);

/// @brief Set the music volume multiplier.
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
