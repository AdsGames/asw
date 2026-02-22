/// @file theme.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief UI theme configuration for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_THEME_H
#define ASW_MODULES_UI_THEME_H

#include "../color.h"

namespace asw::ui {

/// @brief Theme configuration for UI elements.
///
struct Theme {
    /// @brief Default text color.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Dimmed text color.
    asw::Color text_dim { 200, 200, 200, 255 };

    /// @brief Panel background color.
    asw::Color panel_bg { 30, 30, 30, 255 };

    /// @brief Button background color.
    asw::Color btn_bg { 55, 55, 55, 255 };

    /// @brief Button hover color.
    asw::Color btn_hover { 75, 75, 75, 255 };

    /// @brief Button pressed color.
    asw::Color btn_pressed { 95, 95, 95, 255 };

    /// @brief Button focus ring color.
    asw::Color btn_focus_ring { 255, 200, 80, 255 };

    /// @brief Input box background color.
    asw::Color input_bg { 20, 20, 20, 255 };

    /// @brief Default padding.
    float padding = 10.0f;

    /// @brief Default gap between elements.
    float gap = 8.0f;

    /// @brief Show focus rings around focused widgets. Enabled by default when using keyboard
    /// navigation.
    bool show_focus = false;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_THEME_H
