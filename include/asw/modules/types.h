#ifndef ASW_TYPES_H
#define ASW_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

namespace asw {
  using Texture = std::shared_ptr<SDL_Texture>;
  using Font = std::shared_ptr<TTF_Font>;
  using Sample = std::shared_ptr<Mix_Chunk>;
  using Renderer = SDL_Renderer;
  using Window = SDL_Window;
  using Color = SDL_Color;

  enum class BlendMode {
    NONE = SDL_BLENDMODE_NONE,
    BLEND = SDL_BLENDMODE_BLEND,
    ADD = SDL_BLENDMODE_ADD,
    MODULATE = SDL_BLENDMODE_MOD,
    MULTIPLY = SDL_BLENDMODE_MUL
  };

}  // namespace asw

#endif  // ASW_TYPES_H