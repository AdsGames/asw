#include "./asw/modules/assets.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

#include "./asw/modules/display.h"

asw::Texture asw::assets::loadTexture(const std::string& filename) {
  SDL_Texture* temp = IMG_LoadTexture(asw::display::renderer, filename.c_str());

  if (!temp) {
    asw::util::abortOnError("Failed to load texture: " + filename);
  }

  return std::shared_ptr<SDL_Texture>(temp, SDL_DestroyTexture);
}

asw::Font asw::assets::loadFont(const std::string& filename, int size) {
  TTF_Font* temp = TTF_OpenFont(filename.c_str(), size);

  if (!temp) {
    asw::util::abortOnError("Failed to load font: " + filename);
  }
  return std::shared_ptr<TTF_Font>(temp, TTF_CloseFont);
}

asw::Sample asw::assets::loadSample(const std::string& filename) {
  Mix_Chunk* temp = Mix_LoadWAV(filename.c_str());

  if (!temp) {
    asw::util::abortOnError("Failed to load sample: " + filename);
  }
  return std::shared_ptr<Mix_Chunk>(temp, Mix_FreeChunk);
}

asw::Texture asw::assets::createTexture(int w, int h) {
  SDL_Texture* text =
      SDL_CreateTexture(asw::display::renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, w, h);
  return std::shared_ptr<SDL_Texture>(text, SDL_DestroyTexture);
}
