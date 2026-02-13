#include "./asw/modules/sound.h"

#include <algorithm>

#include <SDL3_mixer/SDL_mixer.h>

namespace {
  float masterVolume = 1.0F;
  float sfxVolume = 1.0F;
  float musicVolume = 1.0F;

  int computeSfxVolume(float requestedVolume) {
    auto volume = requestedVolume * 255.0F * sfxVolume * masterVolume;
    return static_cast<int>(volume);
  }

  int computeMusicVolume(float requestedVolume) {
    auto volume = requestedVolume * 255.0F * musicVolume * masterVolume;
    return static_cast<int>(volume);
  }
}  // namespace

void asw::sound::play(const asw::Sample& sample,
                      float volume,
                      float pan,
                      bool loop) {
  const int channel = Mix_PlayChannel(-1, sample.get(), loop ? -1 : 0);
  if (channel >= 0) {
    Mix_Volume(channel, computeSfxVolume(volume));

    auto panning = (std::clamp(pan, -1.0F, 1.0F) + 1.0F) * 127.5F;
    auto int_pan = static_cast<int>(panning);

    Mix_SetPanning(channel, 255 - int_pan, int_pan);
  }
}

void asw::sound::playMusic(const asw::Music& sample, float volume) {
  Mix_VolumeMusic(computeMusicVolume(volume));
  Mix_PlayMusic(sample.get(), -1);
}

void asw::sound::stopMusic() {
  Mix_HaltMusic();
}

void asw::sound::fadeInMusic(const asw::Music& music,
                             float volume,
                             int durationMs) {
  Mix_FadeInMusic(music.get(), -1, durationMs);
  Mix_VolumeMusic(computeMusicVolume(volume));
}

void asw::sound::fadeOutMusic(int durationMs) {
  Mix_FadeOutMusic(durationMs);
}

void asw::sound::pauseMusic() {
  Mix_PauseMusic();
}

void asw::sound::resumeMusic() {
  Mix_ResumeMusic();
}

bool asw::sound::isMusicPlaying() {
  return Mix_PlayingMusic();
}

bool asw::sound::isMusicPaused() {
  return Mix_PausedMusic();
}

void asw::sound::setMasterVolume(float volume) {
  masterVolume = std::clamp(volume, 0.0F, 1.0F);
}

void asw::sound::setSfxVolume(float volume) {
  sfxVolume = std::clamp(volume, 0.0F, 1.0F);
}

void asw::sound::setMusicVolume(float volume) {
  musicVolume = std::clamp(volume, 0.0F, 1.0F);
}

float asw::sound::getMasterVolume() {
  return masterVolume;
}

float asw::sound::getSfxVolume() {
  return sfxVolume;
}

float asw::sound::getMusicVolume() {
  return musicVolume;
}
