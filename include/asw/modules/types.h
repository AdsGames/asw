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
#include <type_traits>

namespace asw {

/// @brief Mappings from SDL_BLENDMODE to ASW BlendMode
///
/// @details The underlying type matches SDL_BlendMode so conversions between
/// the two are casts to the underlying type rather than to an unrelated
/// integral type.
///
enum class BlendMode : SDL_BlendMode {
    None = SDL_BLENDMODE_NONE,
    Blend = SDL_BLENDMODE_BLEND,
    BlendPremultiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    Add = SDL_BLENDMODE_ADD,
    AddPremultiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    Modulate = SDL_BLENDMODE_MOD,
    Multiply = SDL_BLENDMODE_MUL,
};

static_assert(std::is_same_v<std::underlying_type_t<BlendMode>, SDL_BlendMode>,
    "BlendMode must share SDL_BlendMode's underlying type");

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

/// @brief Alias for a shared pointer to an MIX_Audio
using Sample = std::shared_ptr<MIX_Audio>;

/// @brief Alias for a shared pointer to an MIX_Audio
using Music = std::shared_ptr<MIX_Audio>;

/// @brief Alias for a shared pointer to an SDL_Renderer
using Renderer = SDL_Renderer;

/// @brief Alias for a shared pointer to an SDL_Window
using Window = SDL_Window;

} // namespace asw

#endif // ASW_TYPES_H