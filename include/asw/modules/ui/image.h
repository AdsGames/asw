/// @file image.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Image widget for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_IMAGE_H
#define ASW_MODULES_UI_IMAGE_H

#include "../types.h"
#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief A texture stretched over the widget, e.g. a logo or icon in a layout.
///
class Image : public Widget {
public:
    /// @brief The texture to draw.
    asw::Texture texture;

    /// @brief Set the texture, optionally resizing the widget to match.
    ///
    /// @param tex The texture.
    /// @param auto_size If true, resizes the widget to the texture size.
    ///
    void set_texture(const asw::Texture& tex, bool auto_size = true);

    /// @brief Draw the image.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_IMAGE_H
