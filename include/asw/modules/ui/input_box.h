/// @file input_box.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Input box widget for the ASW UI module
/// @date 2026-02-22
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_INPUT_BOX_H
#define ASW_MODULES_UI_INPUT_BOX_H

#include <functional>
#include <string>

#include "../types.h"
#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief A text input widget.
///
class InputBox : public Widget {
public:
    /// @brief Default constructor.
    ///
    InputBox()
    {
        focusable = true;
    }

    /// @brief Callback invoked when the value changes.
    std::function<void(const std::string&)> on_change;

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

    /// @brief Draw the input box.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

    /// @brief The font to use for the input text.
    asw::Font font;

    /// @brief The current text value.
    std::string value;

    /// @brief Placeholder text shown when value is empty.
    std::string placeholder;

private:
    bool hovered_ = false;
    bool focused_ = false;
    std::size_t cursor_pos_ = 0;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_INPUT_BOX_H
