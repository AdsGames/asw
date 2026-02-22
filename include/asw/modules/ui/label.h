/// @file label.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Label widget for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_LABEL_H
#define ASW_MODULES_UI_LABEL_H

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

    /// @brief The font to use for rendering.
    asw::Font font;

    /// @brief The text to display.
    std::string text;

    /// @brief The text justification.
    asw::TextJustify justify = asw::TextJustify::Left;

    /// @brief The text color.
    asw::Color color;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_LABEL_H
