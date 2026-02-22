/// @file context.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief UI context and focus management for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_UI_CONTEXT_H
#define ASW_UI_CONTEXT_H

#include <vector>

#include "theme.h"
#include "widget.h"

namespace asw::ui {

/// @brief Manages focus navigation for UI widgets.
///
class FocusManager {
public:
    /// @brief Rebuild the focusable widget list from the widget tree.
    ///
    /// @param root The root widget to traverse.
    ///
    void rebuild(Widget& root);

    /// @brief Get the currently focused widget.
    ///
    /// @return Pointer to the focused widget, or nullptr.
    ///
    Widget* focused() const
    {
        return focused_;
    }

    /// @brief Set focus to a specific widget.
    ///
    /// @param ctx The UI context.
    /// @param w The widget to focus.
    ///
    void set_focus(Context& ctx, Widget* w);

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

    /// @brief Move focus in a direction based on widget positions.
    ///
    /// @param ctx The UI context.
    /// @param dx Horizontal direction (-1, 0, or 1).
    /// @param dy Vertical direction (-1, 0, or 1).
    ///
    void focus_dir(Context& ctx, int dx, int dy);

private:
    void dfs(Widget& w);

    std::vector<Widget*> focusables_;
    Widget* focused_ = nullptr;
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

    /// @brief The widget that has captured pointer input.
    Widget* pointer_capture = nullptr;

    /// @brief The widget currently being hovered.
    Widget* hover = nullptr;

    /// @brief Whether the pointer is currently down.
    bool pointer_down = false;

    /// @brief Whether the focus list needs to be rebuilt.
    bool need_focus_rebuild = true;
};

} // namespace asw::ui

#endif // ASW_UI_CONTEXT_H
