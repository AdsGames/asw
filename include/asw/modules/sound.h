#ifndef ASW_SOUND_H
#define ASW_SOUND_H

#include "./types.h"

namespace asw::sound {
  void play(asw::Sample sample,
            int volume = 255,
            unsigned char pan = 128,
            int loop = 0);
}  // namespace asw::sound

#endif  // ASW_SOUND_H