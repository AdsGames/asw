/**
 * @file sound.h
 * @author Allan Legemaate (alegemaate@gmail.com)
 * @brief Sound module for the ASW library
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ASW_SOUND_H
#define ASW_SOUND_H

#include "./types.h"

namespace asw::sound {
  /**
   * @brief Play a sample.
   *
   * @param sample Sample to play
   * @param volume Playback volume
   * @param pan Panning (0-255)
   * @param loop Number of times to loop (0 = no loop, -1 = infinite loop)
   */
  void play(asw::Sample sample,
            int volume = 255,
            unsigned char pan = 128,
            int loop = 0);
}  // namespace asw::sound

#endif  // ASW_SOUND_H