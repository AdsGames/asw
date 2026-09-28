/// @file root.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Root UI container and event dispatcher for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_UI_ROOT_H
#define ASW_UI_ROOT_H

#include <functional>
#include <vector>

#include "context.h"
#include "panel.h"

namespace asw::ui {

/// @brief Root container that manages the UI tree, input, and rendering.
///
/// @details Call update() once per frame after asw::core::update(), then
/// draw(). Root tracks hover, press, focus and the focus ring for every
/// widget, and reads navigation from Context::navigation.
///
class Root {
public:
    /// @brief Default constructor.
    ///
    Root();

    /// @brief The UI context.
    Context ctx;

    /// @brief The root panel widget. See through by default, set bg for a
    /// background.
    Panel root;

    /// @brief Called when back is pressed and the focused widget does not
    /// handle it, e.g. to leave a menu.
    std::function<void()> on_back;

    /// @brief Keep the root panel the size of the logical screen. Turned off
    /// by set_size.
    bool auto_size = true;

    /// @brief Set a fixed size for the root panel.
    ///
    /// @param w The width.
    /// @param h The height.
    ///
    void set_size(float w, float h);

    /// @brief Process input for this frame.
    ///
    /// @return True if the UI used input this frame: the pointer is over a
    /// widget or pressing one, or a navigation, activate, back or text key
    /// went to the UI. Games can skip their own input for the frame.
    ///
    bool update();

    /// @brief Draw the UI tree, then the focus ring.
    ///
    void draw();

    /// @brief Focus a widget.
    ///
    /// @param w The widget, it must be focusable and in this tree.
    /// @param show Show the focus ring, as keyboard navigation would.
    ///
    void focus(Widget& w, bool show = false);

    /// @brief Remove focus from every widget.
    ///
    void clear_focus();

private:
    void fit_to_screen();
    bool attached(const Widget* w) const;
    void validate();
    void free_removed(Widget& w);
    Widget* hit_test(Widget& w, const asw::Vec2<float>& pointer_pos);
    bool bubble(Widget* target, const UIEvent& e);
    bool dispatch_pointer(const UIEvent& e);
    bool dispatch_to_focused(const UIEvent& e);
    void update_pointer();
    void update_keys();
    void activate(Widget& w);

    bool _used = false;
};

} // namespace asw::ui

#endif // ASW_UI_ROOT_H
