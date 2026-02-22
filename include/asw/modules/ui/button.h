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

    /// @brief Called when focus state changes.
    ///
    /// @param ctx The UI context.
    /// @param focused Whether the widget is now focused.
    ///
    void on_focus_changed(Context& ctx, bool focused) override;

    /// @brief Handle a UI event.
    ///
    /// @param ctx The UI context.
    /// @param e The event to handle.
    /// @return True if the event was handled.
    ///
    bool on_event(Context& ctx, const UIEvent& e) override;

    /// @brief Draw the button.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

    /// @brief The font to use for the button text.
    asw::Font font;

    /// @brief The button text.
    std::string text;

private:
    bool hovered_ = false;
    bool pressed_ = false;
    bool focused_ = false;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_BUTTON_H
