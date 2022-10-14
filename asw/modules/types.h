#ifndef ASW_TYPES_H
#define ASW_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

namespace asw {
  typedef std::shared_ptr<SDL_Texture> Texture;
  typedef std::shared_ptr<TTF_Font> Font;
  typedef std::shared_ptr<Mix_Chunk> Sample;
  typedef SDL_Renderer Renderer;
  typedef SDL_Window Window;
  typedef SDL_Color Color;
}  // namespace asw

#endif  // ASW_TYPES_H