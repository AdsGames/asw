#include "./sound.h"

#include <SDL2/SDL_mixer.h>

void asw::sound::play(asw::Sample sample, int volume, int pan, int loop) {
  int channel = Mix_GroupAvailable(0);
  Mix_VolumeChunk(sample.get(), volume);
  Mix_SetPanning(channel, pan, 255 - pan);
  Mix_PlayChannel(-1, sample.get(), loop);
}