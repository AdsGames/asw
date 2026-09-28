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

    /// @brief The font for the text. Uses the theme font when empty.
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

    /// @brief Make an image button: set the state textures, turn off the
    /// background, and optionally size the button to the normal texture.
    ///
    /// @param normal Texture when idle.
    /// @param hover Texture while hovered or focused, optional.
    /// @param pressed Texture while pressed, optional.
    /// @param disabled Texture while disabled, optional.
    /// @param auto_size If true, resizes the button to the normal texture.
    ///
    void set_images(const asw::Texture& normal, const asw::Texture& hover = nullptr,
        const asw::Texture& pressed = nullptr, const asw::Texture& disabled = nullptr,
        bool auto_size = true);

    /// @brief Set the text, optionally resizing the button to match.
    ///
    /// @details Without its own font the button is sized with the theme font
    /// when it is next measured, before its parent places it.
    ///
    /// @param t The text to set.
    /// @param auto_size If true, resizes the button to fit the text.
    ///
    void set_text(const std::string& t, bool auto_size = false);

    /// @brief Size to the text if set_text asked for it.
    ///
    /// @param ctx The UI context.
    ///
    void measure(Context& ctx) override;

    /// @brief Measure, in case the parent did not, then lay out the children.
    ///
    /// @param ctx The UI context.
    ///
    void layout(Context& ctx) override;

protected:
    /// @brief Get the texture for the current state.
    ///
    /// @param ctx The UI context.
    /// @return The texture to draw, may be nullptr.
    ///
    const asw::Texture& current_texture(const Context& ctx) const;

private:
    // set_text asked to fit the text, waiting for the theme font at layout
    bool _fit_text = false;

    void fit_text(const asw::Font& f);
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_BUTTON_H
