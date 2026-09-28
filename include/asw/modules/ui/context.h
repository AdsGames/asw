/// @file context.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief UI context and focus management for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_UI_CONTEXT_H
#define ASW_UI_CONTEXT_H

#include <optional>
#include <vector>

#include "navigation.h"
#include "theme.h"
#include "widget.h"

namespace asw::ui {

/// @brief Manages focus navigation for UI widgets.
///
class FocusManager {
public:
    /// @brief Get the currently focused widget.
    ///
    /// @return Pointer to the focused widget, or nullptr.
    ///
    Widget* focused() const
    {
        return _focused;
    }

    /// @brief Set focus to a specific widget.
    ///
    /// @param ctx The UI context.
    /// @param w The widget to focus.
    ///
    void set_focus(Context& ctx, Widget* w);

    /// @brief Widget focused by the first navigation press when nothing has
    /// focus. The first focusable widget when null.
    Widget* default_focus = nullptr;

    /// @brief Move focus to the next focusable widget.
    ///
    /// @param ctx The UI context.
    ///
    void focus_next(Context& ctx);

    /// @brief Move focus to the previous focusable widget.
    ///
    /// @param ctx The UI context.
    ///
    void focus_prev(Context& ctx);

    /// @brief Focus default_focus, or the first focusable widget, when
    /// nothing has focus.
    /// @param ctx The UI context.
    /// @return True if focus was set.
    bool focus_start(Context& ctx);

    /// @brief Move focus in a direction based on widget positions.
    ///
    /// @details A widget's nav_up, nav_down, nav_left or nav_right wins when
    /// set. Otherwise widgets in the same lane (overlapping across the
    /// direction) win over the rest, then the nearest edge to edge, then the
    /// one closest to the lane repeated moves started in.
    ///
    /// @param ctx The UI context.
    /// @param dx Horizontal direction (-1, 0, or 1).
    /// @param dy Vertical direction (-1, 0, or 1).
    ///
    void focus_dir(Context& ctx, int dx, int dy);

private:
    friend class Root;

    // Rebuild the focusable widget list from the widget tree
    void rebuild(Context& ctx, Widget& root);

    void dfs(Widget& w);

    std::vector<Widget*> _focusables;
    Widget* _focused = nullptr;

    // Across position kept during repeated moves one way
    std::optional<float> _lane_x;
    std::optional<float> _lane_y;
};

/// @brief Shared state for the UI system.
///
class Context {
public:
    /// @brief Default constructor.
    ///
    Context() = default;

    /// @brief The current UI theme.
    Theme theme;

    /// @brief The focus manager.
    FocusManager focus;

    /// @brief Actions to read for focus navigation. Empty names use the built
    /// in keys.
    Navigation navigation;

    /// @brief Whether focus is shown. Root turns it on for keyboard and
    /// controller navigation and off when the mouse is used.
    bool show_focus = false;

private:
    friend class Root;

    // Widget pressed by the pointer, it gets pointer events until release
    Widget* pointer_capture = nullptr;

    // Widget under the pointer
    Widget* hover = nullptr;

    // Focus list needs to be rebuilt
    bool need_focus_rebuild = true;
};

} // namespace asw::ui

#endif // ASW_UI_CONTEXT_H
