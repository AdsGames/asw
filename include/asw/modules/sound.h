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
  /// @brief Play a sample.
  ///
  /// @param sample Sample to play
  /// @param volume Playback volume (0.0 - 1.0).
  /// @param pan Panning (-1.0 - 1.0), where -1.0 is full left, 0.0 is center,
  /// and 1.0 is full right.
  /// @param loop Whether to loop the sample (false = no loop, true = infinite
  /// loop).
  ///
  void play(const asw::Sample& sample,
            float volume = 1.0F,
            float pan = 0.0F,
            bool loop = false);

  /// @brief Play a music sample.
  ///
  /// @param sample Sample to play
  /// @param volume Playback volume (0.0 - 1.0).
  ///
  void playMusic(const asw::Music& sample, float volume = 1.0F);

  /// @brief Stop the currently playing music.
  ///
  void stopMusic();

  /// @brief Fade in music over a duration.
  ///
  /// @param music The music to play.
  /// @param volume Playback volume (0.0 - 1.0).
  /// @param durationMs Fade duration in milliseconds.
  ///
  void fadeInMusic(const asw::Music& music, float volume, int durationMs);

  /// @brief Fade out the currently playing music.
  ///
  /// @param durationMs Fade duration in milliseconds.
  ///
  void fadeOutMusic(int durationMs);

  /// @brief Pause the currently playing music.
  ///
  void pauseMusic();

  /// @brief Resume paused music.
  ///
  void resumeMusic();

  /// @brief Check if music is currently playing.
  ///
  /// @return True if music is playing.
  ///
  bool isMusicPlaying();

  /// @brief Check if music is paused.
  ///
  /// @return True if music is paused.
  ///
  bool isMusicPaused();

  /// @brief Set the master volume multiplier (affects all audio).
  ///
  /// @param volume Volume multiplier (0.0 - 1.0).
  ///
  void setMasterVolume(float volume);

  /// @brief Set the SFX volume multiplier.
  ///
  /// @param volume Volume multiplier (0.0 - 1.0).
  ///
  void setSfxVolume(float volume);

  /// @brief Set the music volume multiplier.
  ///
  /// @param volume Volume multiplier (0.0 - 1.0).
  ///
  void setMusicVolume(float volume);

  /// @brief Get the current master volume multiplier.
  ///
  /// @return The master volume (0.0 - 1.0).
  ///
  float getMasterVolume();

  /// @brief Get the current SFX volume multiplier.
  ///
  /// @return The SFX volume (0.0 - 1.0).
  ///
  float getSfxVolume();

  /// @brief Get the current music volume multiplier.
  ///
  /// @return The music volume (0.0 - 1.0).
  ///
  float getMusicVolume();

}  // namespace asw::sound

#endif  // ASW_SOUND_H
