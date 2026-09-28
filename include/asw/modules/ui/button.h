/// @file button.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Button widget for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_BUTTON_H
#define ASW_MODULES_UI_BUTTON_H

#include <functional>
#include <optional>
#include <string>

#include "../types.h"
#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief An interactive button widget.
///
class Button : public Widget {
public:
    /// @brief Default constructor.
    ///
    Button()
    {
        focusable = true;
    }

    /// @brief Callback invoked when the button is clicked.
    std::function<void()> on_click;

    /// @brief Calls on_click.
    ///
    /// @param ctx The UI context.
    ///
    void activate(Context& ctx) override;

    /// @brief Draw the button.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

    /// @brief Padding applied inside the button on all sides.
    float padding = 0.0f;

    /// @brief The font to use for the button text.
    asw::Font font;

    /// @brief The button text.
    std::string text;

    /// @brief The texture to display on the button.
    asw::Texture texture;

    /// @brief Texture shown while hovered or focused. Falls back to texture.
    asw::Texture texture_hover;

    /// @brief Texture shown while pressed. Falls back to texture_hover, then texture.
    asw::Texture texture_pressed;

    /// @brief Texture shown while disabled. Falls back to texture.
    asw::Texture texture_disabled;

    /// @brief Fill the button with the theme background. Turn off for buttons
    /// that are only an image.
    bool draw_background = true;

    /// @brief Style for this button only. Uses the theme button style when empty.
    std::optional<ButtonStyle> style;

    /// @brief Get the style this button draws with.
    ///
    /// @param ctx The UI context.
    /// @return The button's own style, or the theme button style.
    ///
    const ButtonStyle& get_style(const Context& ctx) const
    {
        return style ? *style : ctx.theme.button;
    }

    /// @brief Set the texture, optionally resizing the button to match.
    ///
    /// @param tex The texture to set.
    /// @param auto_size If true, resizes the button to the texture dimensions.
    ///
    void set_texture(const asw::Texture& tex, bool auto_size = false);

    /// @brief Set the text, optionally resizing the button to match.
    ///
    /// @param t The text to set.
    /// @param auto_size If true, resizes the button to fit the text.
    ///
    void set_text(const std::string& t, bool auto_size = false);

protected:
    /// @brief Get the texture for the current state.
    ///
    /// @param ctx The UI context.
    /// @return The texture to draw, may be nullptr.
    ///
    const asw::Texture& current_texture(const Context& ctx) const;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_BUTTON_H
