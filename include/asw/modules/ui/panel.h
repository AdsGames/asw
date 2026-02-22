/// @file panel.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Panel widget for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_PANEL_H
#define ASW_MODULES_UI_PANEL_H

#include "../types.h"
#include "context.h"
#include "widget.h"

namespace asw::ui {

  /// @brief A container widget with a background color or image.
  ///
  class Panel : public Widget {
   public:
    /// @brief Default constructor.
    ///
    Panel() { focusable = false; }

    /// @brief The background color.
    asw::Color bg{0, 0, 0, 0};

    /// @brief The background image texture.
    asw::Texture bg_image;

    /// @brief Draw the panel and its children.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;
  };

}  // namespace asw::ui

#endif  // ASW_MODULES_UI_PANEL_H
