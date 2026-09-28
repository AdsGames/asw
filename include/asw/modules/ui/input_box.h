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
#include <optional>
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

    /// @brief Stops text input if the box is destroyed while focused.
    ///
    ~InputBox() override;

    InputBox(const InputBox&) = delete;
    InputBox& operator=(const InputBox&) = delete;

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

    /// @brief Style for this input box only. Uses the theme input style when empty.
    std::optional<InputStyle> style;

    /// @brief Get the style this input box draws with.
    ///
    /// @param ctx The UI context.
    /// @return The input box's own style, or the theme input style.
    ///
    const InputStyle& get_style(const Context& ctx) const
    {
        return style ? *style : ctx.theme.input;
    }

private:
    std::size_t _cursor_pos = 0;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_INPUT_BOX_H
