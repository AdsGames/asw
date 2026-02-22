#include "./asw/modules/sound.h"

#include <algorithm>

#include <SDL3_mixer/SDL_mixer.h>

namespace {
  float master_volume = 1.0F;
  float sfx_volume = 1.0F;
  float music_volume = 1.0F;

  int compute_sfx_volume(float requestedVolume) {
    auto volume = requestedVolume * 255.0F * sfx_volume * master_volume;
    return static_cast<int>(volume);
  }

  int compute_music_volume(float requestedVolume) {
    auto volume = requestedVolume * 255.0F * music_volume * master_volume;
    return static_cast<int>(volume);
  }
}  // namespace

void asw::sound::play(const asw::Sample& sample,
                      float volume,
                      float pan,
                      bool loop) {
  const int channel = Mix_PlayChannel(-1, sample.get(), loop ? -1 : 0);
  if (channel >= 0) {
    Mix_Volume(channel, compute_sfx_volume(volume));

    auto panning = (std::clamp(pan, -1.0F, 1.0F) + 1.0F) * 127.5F;
    auto int_pan = static_cast<unsigned char>(panning);

    Mix_SetPanning(channel, 255 - int_pan, int_pan);
  }
}

void asw::sound::play_music(const asw::Music& sample, float volume) {
  Mix_VolumeMusic(compute_music_volume(volume));
  Mix_PlayMusic(sample.get(), -1);
}

void asw::sound::stop_music() {
  Mix_HaltMusic();
}

void asw::sound::fade_in_music(const asw::Music& music,
                               float volume,
                               float duration) {
  Mix_FadeInMusic(music.get(), -1, static_cast<int>(duration * 1000.0F));
  Mix_VolumeMusic(compute_music_volume(volume));
}

void asw::sound::fade_out_music(float duration) {
  Mix_FadeOutMusic(static_cast<int>(duration * 1000.0F));
}

void asw::sound::pause_music() {
  Mix_PauseMusic();
}

void asw::sound::resume_music() {
  Mix_ResumeMusic();
}

bool asw::sound::is_music_playing() {
  return Mix_PlayingMusic();
}

bool asw::sound::is_music_paused() {
  return Mix_PausedMusic();
}

void asw::sound::set_master_volume(float volume) {
  master_volume = std::clamp(volume, 0.0F, 1.0F);
}

void asw::sound::set_sfx_volume(float volume) {
  sfx_volume = std::clamp(volume, 0.0F, 1.0F);
}

void asw::sound::set_music_volume(float volume) {
  music_volume = std::clamp(volume, 0.0F, 1.0F);
}

float asw::sound::get_master_volume() {
  return master_volume;
}

float asw::sound::get_sfx_volume() {
  return sfx_volume;
}

float asw::sound::get_music_volume() {
  return music_volume;
}
