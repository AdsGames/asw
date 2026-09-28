/// @file root.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Root UI container and event dispatcher for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_UI_ROOT_H
#define ASW_UI_ROOT_H

#include "context.h"
#include "panel.h"

namespace asw::ui {

/// @brief Root container that manages the UI tree, input, and rendering.
///
class Root {
public:
    /// @brief Default constructor.
    ///
    Root();

    /// @brief The UI context.
    Context ctx;

    /// @brief The root panel widget.
    Panel root;

    /// @brief Set the size of the root panel.
    ///
    /// @param w The width.
    /// @param h The height.
    ///
    void set_size(float w, float h);

    /// @brief Rebuild the focus list if need_focus_rebuild is set. The tree
    /// is also checked automatically every update and after each dispatched
    /// event, so this is rarely needed.
    ///
    void rebuild_focus_if_needed();

    /// @brief Drop hover, capture and focus pointers to widgets that are no
    /// longer in the tree, and refresh the focus list.
    ///
    void validate();

    /// @brief Find the deepest widget at a given pointer position.
    ///
    /// @param w The widget to test.
    /// @param pointer_pos The pointer position.
    /// @return Pointer to the hit widget, or nullptr.
    ///
    Widget* hit_test(Widget& w, const asw::Vec2<float>& pointer_pos);

    /// @brief Route a pointer event to the appropriate widget.
    ///
    /// @param e The UI event to dispatch.
    /// @return True if the event was handled.
    ///
    bool dispatch_pointer(const UIEvent& e);

    /// @brief Dispatch an event to the currently focused widget.
    ///
    /// @param e The UI event to dispatch.
    /// @return True if the event was handled.
    ///
    bool dispatch_to_focused(const UIEvent& e);

    /// @brief Process input and dispatch UI events.
    ///
    void update();

    /// @brief Draw the UI tree.
    ///
    void draw();

private:
    std::vector<Widget*> _live;
};

} // namespace asw::ui

#endif // ASW_UI_ROOT_H
