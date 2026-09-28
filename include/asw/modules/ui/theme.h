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
#include "../geometry.h"
#include "../types.h"

namespace asw::ui {

/// @brief How a button looks. Set on Theme::button for every button, or on
/// Button::style for one button.
///
struct ButtonStyle {
    /// @brief Background color.
    asw::Color bg { 55, 55, 55, 255 };

    /// @brief Background color while hovered, or focused with focus shown.
    asw::Color bg_hover { 75, 75, 75, 255 };

    /// @brief Background color while pressed.
    asw::Color bg_pressed { 95, 95, 95, 255 };

    /// @brief Background color while disabled.
    asw::Color bg_disabled { 30, 30, 30, 255 };

    /// @brief Text color.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Text color while hovered, or focused with focus shown.
    asw::Color text_hover { 255, 255, 255, 255 };

    /// @brief Text color while disabled.
    asw::Color text_disabled { 200, 200, 200, 255 };

    /// @brief Border color.
    asw::Color border { 0, 0, 0, 0 };

    /// @brief Border width in pixels, drawn inside the button. 0 draws no border.
    float border_width = 0.0F;

    /// @brief Where the text sits. Left and right are inset by the button padding.
    asw::TextJustify text_align = asw::TextJustify::Center;
};

/// @brief Which side of a checkbox the box is on.
///
enum class BoxSide {
    Left,
    Right,
};

/// @brief How a checkbox looks. Set on Theme::checkbox for every checkbox, or
/// on Checkbox::checkbox_style for one checkbox.
///
struct CheckboxStyle {
    /// @brief Background color of the whole row.
    asw::Color bg { 0, 0, 0, 0 };

    /// @brief Row background color while hovered, or focused with focus shown.
    asw::Color bg_hover { 0, 0, 0, 0 };

    /// @brief Row border color.
    asw::Color border { 0, 0, 0, 0 };

    /// @brief Row border width in pixels. 0 draws no border.
    float border_width = 0.0F;

    /// @brief Box color.
    asw::Color box { 55, 55, 55, 255 };

    /// @brief Box color while hovered, or focused with focus shown.
    asw::Color box_hover { 75, 75, 75, 255 };

    /// @brief Box color while pressed.
    asw::Color box_pressed { 95, 95, 95, 255 };

    /// @brief Box color while disabled.
    asw::Color box_disabled { 30, 30, 30, 255 };

    /// @brief Box border color.
    asw::Color box_border { 0, 0, 0, 0 };

    /// @brief Box border width in pixels. 0 draws no border.
    float box_border_width = 0.0F;

    /// @brief Check mark color.
    asw::Color mark { 255, 255, 255, 255 };

    /// @brief Check mark color while disabled.
    asw::Color mark_disabled { 200, 200, 200, 255 };

    /// @brief Text color.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Text color while disabled.
    asw::Color text_disabled { 200, 200, 200, 255 };

    /// @brief Which side the box is on. The text is on the other side.
    BoxSide box_side = BoxSide::Left;
};

/// @brief How an input box looks. Set on Theme::input for every input box, or
/// on InputBox::style for one input box.
///
struct InputStyle {
    /// @brief Background color.
    asw::Color bg { 20, 20, 20, 255 };

    /// @brief Background color while disabled.
    asw::Color bg_disabled { 30, 30, 30, 255 };

    /// @brief Border color.
    asw::Color border { 55, 55, 55, 255 };

    /// @brief Border color while hovered.
    asw::Color border_hover { 75, 75, 75, 255 };

    /// @brief Border width in pixels. 0 draws no border.
    float border_width = 1.0F;

    /// @brief Text color.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Placeholder text color.
    asw::Color placeholder { 200, 200, 200, 255 };

    /// @brief Text cursor color.
    asw::Color caret { 255, 255, 255, 255 };
};

/// @brief The ring drawn around the focused widget during keyboard or
/// controller navigation.
///
struct FocusRingStyle {
    /// @brief Ring color.
    asw::Color color { 255, 200, 80, 255 };

    /// @brief Ring width in pixels. 0 draws no ring.
    float width = 1.0F;

    /// @brief Gap between the widget and the ring, in pixels.
    float offset = 2.0F;
};

/// @brief Theme configuration for UI elements.
///
struct Theme {
    /// @brief Default text color.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Dimmed text color.
    asw::Color text_dim { 200, 200, 200, 255 };

    /// @brief Panel background color, also used for the root panel.
    asw::Color panel_bg { 30, 30, 30, 255 };

    /// @brief Default button style.
    ButtonStyle button {};

    /// @brief Default checkbox style.
    CheckboxStyle checkbox {};

    /// @brief Default input box style.
    InputStyle input {};

    /// @brief Focus ring style.
    FocusRingStyle focus_ring {};

    /// @brief Default padding.
    float padding = 10.0f;

    /// @brief Default gap between elements.
    float gap = 8.0f;
};

/// @brief Draw a focus ring around a widget.
///
/// @param style The ring style.
/// @param bounds The widget bounds.
///
void draw_focus_ring(const FocusRingStyle& style, const asw::Quad<float>& bounds);

} // namespace asw::ui

#endif // ASW_MODULES_UI_THEME_H
