/// @file checkbox.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Checkbox widget for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_CHECKBOX_H
#define ASW_MODULES_UI_CHECKBOX_H

#include <functional>

#include "button.h"

namespace asw::ui {

/// @brief A button that toggles between checked and unchecked.
///
/// @details Clicking or activating it flips checked, then calls on_change and
/// on_click. Without textures it draws a box with the text beside it, styled
/// by CheckboxStyle. With textures, texture is the unchecked image and
/// texture_checked the checked one, the hover textures work like Button's.
///
class Checkbox : public Button {
public:
    /// @brief Style for this checkbox only. Uses the theme checkbox style when
    /// empty. The inherited Button::style is not used.
    std::optional<CheckboxStyle> checkbox_style;

    /// @brief Get the style this checkbox draws with.
    ///
    /// @param ctx The UI context.
    /// @return The checkbox's own style, or the theme checkbox style.
    ///
    const CheckboxStyle& get_checkbox_style(const Context& ctx) const
    {
        return checkbox_style ? *checkbox_style : ctx.theme.checkbox;
    }

    /// @brief Whether the box is checked.
    bool checked = false;

    /// @brief Callback invoked when checked changes by user input.
    std::function<void(bool)> on_change;

    /// @brief Texture shown when checked. Falls back to texture.
    asw::Texture texture_checked;

    /// @brief Texture shown when checked and hovered or focused. Falls back to
    /// texture_checked.
    asw::Texture texture_checked_hover;

    /// @brief Draw the checkbox.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

protected:
    /// @brief Toggle checked and notify listeners.
    ///
    void activate() override;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_CHECKBOX_H
