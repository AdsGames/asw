#include "./asw/modules/assets.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "./asw/modules/display.h"
#include "./asw/modules/types.h"
#include "./asw/modules/util.h"

asw::Texture asw::assets::loadTexture(const std::string& filename) {
  SDL_Texture* temp = IMG_LoadTexture(asw::display::renderer, filename.c_str());

  if (temp == nullptr) {
    asw::util::abortOnError("Failed to load texture: " + filename);
  }

  SDL_SetTextureScaleMode(temp, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureBlendMode(temp, SDL_BLENDMODE_BLEND);

  return {temp, SDL_DestroyTexture};
}

asw::Font asw::assets::loadFont(const std::string& filename, float size) {
  TTF_Font* temp = TTF_OpenFont(filename.c_str(), size);

  if (temp == nullptr) {
    asw::util::abortOnError("Failed to load font: " + filename);
  }

  return {temp, TTF_CloseFont};
}

asw::Sample asw::assets::loadSample(const std::string& filename) {
  Mix_Chunk* temp = Mix_LoadWAV(filename.c_str());

  if (temp == nullptr) {
    asw::util::abortOnError("Failed to load sample: " + filename);
  }

  return {temp, Mix_FreeChunk};
}

asw::Music asw::assets::loadMusic(const std::string& filename) {
  Mix_Music* temp = Mix_LoadMUS(filename.c_str());

  if (temp == nullptr) {
    asw::util::abortOnError("Failed to load music: " + filename);
  }

  return {temp, Mix_FreeMusic};
}

asw::Texture asw::assets::createTexture(int w, int h) {
  if (asw::display::renderer == nullptr) {
    asw::util::abortOnError("Renderer not initialized");
  }

  SDL_Texture* text =
      SDL_CreateTexture(asw::display::renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, w, h);

  return {text, SDL_DestroyTexture};
}
