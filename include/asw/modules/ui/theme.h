/// @file theme.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief UI theme configuration for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_THEME_H
#define ASW_MODULES_UI_THEME_H

#include <cstddef>

#include "../color.h"
#include "../geometry.h"
#include "../types.h"
#include "widget.h"

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

/// @brief How a slider looks. Set on Theme::slider for every slider, or on
/// Slider::style for one slider.
///
struct SliderStyle {
    /// @brief Track color.
    asw::Color track { 55, 55, 55, 255 };

    /// @brief Color of the track up to the value.
    asw::Color fill { 255, 200, 80, 255 };

    /// @brief Knob color.
    asw::Color knob { 200, 200, 200, 255 };

    /// @brief Knob color while hovered, pressed, or focused with focus shown.
    asw::Color knob_hover { 255, 255, 255, 255 };

    /// @brief Knob color while editing with left and right, see Slider.
    asw::Color knob_editing { 255, 200, 80, 255 };

    /// @brief Knob and fill color while disabled.
    asw::Color disabled { 90, 90, 90, 255 };

    /// @brief Track height in pixels, centred in the widget.
    float track_height = 4.0F;

    /// @brief Knob width in pixels. The knob is as tall as the widget.
    float knob_width = 10.0F;
};

/// @brief The ring drawn around the focused widget during keyboard or
/// controller navigation.
///
/// @brief Look of the messages Root::toast shows at the top of the screen.
///
struct ToastStyle {
    /// @brief Fill behind each message.
    asw::Color bg { 20, 20, 20, 230 };

    /// @brief Message text.
    asw::Color text { 255, 255, 255, 255 };

    /// @brief Outline, none when the width is 0.
    asw::Color border { 0, 0, 0, 0 };

    /// @brief Outline width.
    float border_width = 0.0F;

    /// @brief Space around the text inside each message.
    float padding = 12.0F;

    /// @brief Space from the top of the screen to the first message.
    float margin = 16.0F;

    /// @brief Space between messages.
    float gap = 8.0F;

    /// @brief How long a message stays, from when it shows.
    float seconds = 3.0F;

    /// @brief How long a message takes to fade out at the end.
    float fade_seconds = 0.25F;

    /// @brief Messages on screen at once, the rest wait their turn.
    std::size_t max_visible = 3;

    /// @brief Where messages show: Top, TopLeft or TopRight stack down from
    /// the top, Bottom, BottomLeft or BottomRight stack up from the bottom.
    Anchor anchor = Anchor::Top;
};

/// @brief Look of a Modal and the dimmed screen behind it.
///
struct ModalStyle {
    /// @brief Colour laid over the screen behind the modal.
    asw::Color dim { 0, 0, 0, 160 };

    /// @brief Fill of the modal.
    asw::Color bg { 30, 30, 30, 255 };

    /// @brief Outline, none when the width is 0.
    asw::Color border { 0, 0, 0, 0 };

    /// @brief Outline width.
    float border_width = 0.0F;

    /// @brief Space between the modal's edge and its content.
    float padding = 20.0F;

    /// @brief Space between its widgets.
    float gap = 12.0F;

    /// @brief Narrowest the modal gets.
    float min_width = 320.0F;
};

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

    /// @brief Panel background color.
    asw::Color panel_bg { 30, 30, 30, 255 };

    /// @brief Font for widgets that do not set their own.
    asw::Font font;

    /// @brief Default button style.
    ButtonStyle button {};

    /// @brief Default checkbox style.
    CheckboxStyle checkbox {};

    /// @brief Default input box style.
    InputStyle input {};

    /// @brief Default slider style.
    SliderStyle slider {};

    /// @brief Focus ring style.
    FocusRingStyle focus_ring {};

    /// @brief Messages from Root::toast.
    ToastStyle toast {};

    /// @brief Modals from Root::open_modal.
    ModalStyle modal {};

    /// @brief Default padding.
    float padding = 10.0f;

    /// @brief Default gap between elements.
    float gap = 8.0f;

    /// @brief Played on the UI bus when navigation moves focus. Optional.
    asw::Sample sound_move;

    /// @brief Played on the UI bus when a widget is clicked or activated.
    /// Optional.
    asw::Sample sound_activate;
};

/// @brief The font a widget draws with: its own, or the theme font.
///
/// @param own The widget's font, may be empty.
/// @param theme The theme.
/// @return The font to use, may be empty if neither is set.
///
inline const asw::Font& pick_font(const asw::Font& own, const Theme& theme)
{
    return own != nullptr ? own : theme.font;
}

/// @brief Draw a focus ring around a widget.
///
/// @param style The ring style.
/// @param bounds The widget bounds.
///
void draw_focus_ring(const FocusRingStyle& style, const asw::Quad<float>& bounds);

} // namespace asw::ui

#endif // ASW_MODULES_UI_THEME_H
