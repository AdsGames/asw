/// @file slider.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Slider widget for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_SLIDER_H
#define ASW_MODULES_UI_SLIDER_H

#include <functional>
#include <optional>

#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief A horizontal slider for a value between min and max, e.g. volume.
///
/// @details Drag or click the track to set the value. While focused, the
/// left and right navigation move it by step. Up and down still move focus.
///
class Slider : public Widget {
public:
    /// @brief Default constructor.
    ///
    Slider()
    {
        focusable = true;
    }

    /// @brief Current value.
    float value = 0.0F;

    /// @brief Smallest value.
    float min = 0.0F;

    /// @brief Largest value.
    float max = 1.0F;

    /// @brief Change per left or right press. Dragging snaps to it when
    /// snap is set.
    float step = 0.1F;

    /// @brief Snap dragged values to step.
    bool snap = false;

    /// @brief Callback invoked with the new value when the user changes it.
    std::function<void(float)> on_change;

    /// @brief Style for this slider only. Uses the theme slider style when empty.
    std::optional<SliderStyle> style;

    /// @brief Get the style this slider draws with.
    ///
    /// @param ctx The UI context.
    /// @return The slider's own style, or the theme slider style.
    ///
    const SliderStyle& get_style(const Context& ctx) const
    {
        return style ? *style : ctx.theme.slider;
    }

    /// @brief Set the value without calling on_change.
    ///
    /// @param v The value, clamped to min and max.
    ///
    void set_value(float v);

    /// @brief Handle dragging and left and right while focused.
    ///
    /// @param ctx The UI context.
    /// @param e The event.
    /// @return True if the event was handled.
    ///
    bool on_event(Context& ctx, const UIEvent& e) override;

    /// @brief Draw the slider.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

private:
    void change(float v);
    void set_from_pointer(float x);
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_SLIDER_H
