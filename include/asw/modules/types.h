/// @file types.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Types used throughout the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_TYPES_H
#define ASW_TYPES_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

namespace asw {

  /// @brief Mappings from SDL_BLENDMODE to ASW BlendMode
  enum class BlendMode {
    NONE = SDL_BLENDMODE_NONE,
    BLEND = SDL_BLENDMODE_BLEND,
    BLEND_PREMULTIPLIED = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    ADD = SDL_BLENDMODE_ADD,
    ADD_PREMULTIPLIED = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    MODULATE = SDL_BLENDMODE_MOD,
    MULTIPLY = SDL_BLENDMODE_MUL,
  };

  /// @brief Alias for a shared pointer to an SDL_Texture
  using Texture = std::shared_ptr<SDL_Texture>;

  /// @brief Alias for a shared pointer to an TTF_Font
  using Font = std::shared_ptr<TTF_Font>;

  /// @brief Alias for a shared pointer to an Mix_Chunk
  using Sample = std::shared_ptr<Mix_Chunk>;

  /// @brief Alias for a shared pointer to an Mix_Music
  using Music = std::shared_ptr<Mix_Music>;

  /// @brief Alias for a shared pointer to an SDL_Renderer
  using Renderer = SDL_Renderer;

  /// @brief Alias for a shared pointer to an SDL_Window
  using Window = SDL_Window;

}  // namespace asw

#endif  // ASW_TYPES_H