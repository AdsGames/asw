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
    None = SDL_BLENDMODE_NONE,
    Blend = SDL_BLENDMODE_BLEND,
    BlendPremultiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    Add = SDL_BLENDMODE_ADD,
    AddPremultiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    Modulate = SDL_BLENDMODE_MOD,
    Multiply = SDL_BLENDMODE_MUL,
};

/// @brief Text justification options for text rendering
enum class TextJustify {
    Left,
    Center,
    Right,
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

} // namespace asw

#endif // ASW_TYPES_H