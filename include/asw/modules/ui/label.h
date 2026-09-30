/// @file label.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Label widget for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_LABEL_H
#define ASW_MODULES_UI_LABEL_H

#include <optional>
#include <string>

#include "../types.h"
#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief A text display widget.
///
class Label : public Widget {
public:
    /// @brief Default constructor.
    ///
    Label()
    {
        focusable = false;
    }

    /// @brief Draw the label.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

    /// @brief Set the text, optionally sizing the label to fit it, so layouts
    /// such as VBox and Modal give it room.
    ///
    /// @param t The text to set.
    /// @param auto_size If true, the label is sized to its text when measured.
    ///
    void set_text(const std::string& t, bool auto_size = false);

    /// @brief Size to the text if set_text asked for it.
    ///
    /// @param ctx The UI context.
    ///
    void measure(Context& ctx) override;

    /// @brief The font. Uses the theme font when empty.
    asw::Font font;

    /// @brief The text to display.
    std::string text;

    /// @brief The text justification.
    asw::TextJustify justify = asw::TextJustify::Left;

    /// @brief The text color. Uses the theme text color when empty.
    std::optional<asw::Color> color;

private:
    bool _fit_text = false;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_LABEL_H
