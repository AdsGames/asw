/// @file vbox.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Vertical box layout widget for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_VBOX_H
#define ASW_MODULES_UI_VBOX_H

#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief A vertical box layout container.
///
class VBox : public Widget {
public:
    /// @brief Gap between child elements.
    float gap = 8.0F;

    /// @brief Padding around the content.
    float padding = 10.0F;

    /// @brief Lay out children vertically.
    ///
    /// @param ctx The UI context.
    ///
    void layout(Context& ctx) override;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_VBOX_H
