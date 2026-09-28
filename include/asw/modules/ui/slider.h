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
/// @details Drag or click the track to set the value. While focused, left
/// and right move it by step and up and down move focus. In a row with other
/// focusable widgets left and right move focus instead: activate the slider
/// to edit it with left and right, then activate or go back to finish.
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

    /// @brief Whether left and right change the value without editing first.
    /// Empty to decide from the layout: off in a row with other focusable
    /// widgets, on otherwise.
    std::optional<bool> adjust_on_left_right;

    /// @brief Whether left and right change the value right now, without
    /// editing.
    ///
    /// @return adjust_on_left_right, or the layout default.
    ///
    bool adjusts_left_right() const;

    /// @brief Whether the slider is in edit mode.
    ///
    /// @return True while left and right edit the value after activating.
    ///
    bool is_editing() const
    {
        return _editing;
    }

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

    /// @brief Enter or leave edit mode when left and right move focus.
    ///
    /// @param ctx The UI context.
    ///
    void activate(Context& ctx) override;

    /// @brief Leave edit mode when focus moves away.
    ///
    /// @param ctx The UI context.
    /// @param focused Whether the widget is now focused.
    ///
    void on_focus_changed(Context& ctx, bool focused) override;

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
    bool _editing = false;

    void change(float v);
    void set_from_pointer(float x);
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_SLIDER_H
