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
/// on_click. Without textures it draws a box on the left with the text beside
/// it. With textures, texture is the unchecked image and texture_checked the
/// checked one, the hover textures work like Button's.
///
class Checkbox : public Button {
public:
    /// @brief Default constructor.
    ///
    Checkbox()
    {
        draw_background = false;
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
