/// @file widget.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Base widget class for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_WIDGET_H
#define ASW_MODULES_UI_WIDGET_H

#include <memory>
#include <type_traits>
#include <vector>

#include "event.h"

namespace asw::ui {

class Context;

/// @brief Unique identifier type for widgets.
///
using WidgetId = uint32_t;

/// @brief Base class for all UI widgets.
///
class Widget {
public:
    /// @brief Default constructor for Widget.
    ///
    Widget()
        : _id(generate_id()) { };

    /// @brief Default virtual destructor.
    ///
    virtual ~Widget() = default;

    /// @brief Move constructor.
    ///
    Widget(Widget&&) = default;

    /// @brief Move assignment operator.
    ///
    Widget& operator=(Widget&&) = default;

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    /// @brief Get the unique identifier for this widget.
    ///
    /// @return The widget's unique identifier.
    ///
    WidgetId id() const
    {
        return _id;
    }

    /// @brief Whether the widget is visible.
    bool visible = true;

    /// @brief Whether the widget is enabled.
    bool enabled = true;

    /// @brief Whether the widget can receive focus. Focusable widgets are
    /// also the ones the pointer can press.
    bool focusable = false;

    /// @brief Draw the theme focus ring around this widget while it has
    /// visible focus. Turn off for widgets that show focus themselves.
    bool focus_ring = true;

    /// @brief Pointer to the parent widget.
    Widget* parent = nullptr;

    /// @brief Child widgets.
    std::vector<std::unique_ptr<Widget>> children;

    /// @brief Lay out this widget and its children.
    ///
    /// @param ctx The UI context.
    ///
    virtual void layout(Context& ctx);

    /// @brief Handle a UI event.
    ///
    /// @param ctx The UI context.
    /// @param e The event to handle.
    /// @return True if the event was handled.
    ///
    virtual bool on_event(Context& ctx, const UIEvent& e);

    /// @brief Called when focus state changes.
    ///
    /// @param ctx The UI context.
    /// @param focused Whether the widget is now focused.
    ///
    virtual void on_focus_changed(Context& ctx, bool focused);

    /// @brief Called when the widget is clicked, or activated with the
    /// keyboard or a controller while focused. Does nothing by default.
    ///
    /// @param ctx The UI context.
    ///
    virtual void activate(Context& ctx);

    /// @brief Draw this widget and its children.
    ///
    /// @param ctx The UI context.
    ///
    virtual void draw(Context& ctx);

    /// @brief Add a child widget.
    ///
    /// @tparam T The type of widget to add. Must derive from Widget.
    /// @tparam Args Constructor argument types.
    /// @param args Constructor arguments forwarded to T.
    /// @return A reference to the newly added child widget.
    ///
    template <class T, class... Args> T& add_child(Args&&... args)
    {
        static_assert(std::is_base_of_v<Widget, T>, "add_child<T>: T must derive from Widget");
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        ptr->parent = this;
        auto& ref = *ptr;
        children.emplace_back(std::move(ptr));
        return ref;
    }

    /// @brief Remove and destroy a child widget. Safe to call from a
    /// callback, Root drops any pointer to it before the next use.
    ///
    /// @param child The child to remove.
    /// @return True if it was a child of this widget.
    ///
    bool remove_child(const Widget& child);

    /// @brief Remove and destroy every child widget.
    ///
    void clear_children();

    /// @brief The transform (position and size) of the widget.
    asw::Quad<float> transform;

    /// @brief Whether the pointer is currently over this widget.
    bool is_hovered() const { return _hovered; }

    /// @brief Whether this widget is currently being pressed.
    bool is_pressed() const { return _pressed; }

    /// @brief Whether this widget currently holds focus.
    bool is_focused() const { return _focused; }

    /// @brief Whether this widget was pressed and the pointer is still held,
    /// even if it has moved off the widget.
    bool is_captured() const { return _captured; }

    /// @brief Whether to draw the hover look: hovered, pressed, or focused
    /// while focus is shown.
    ///
    /// @param ctx The UI context.
    /// @return True if the hover look should be drawn.
    ///
    bool is_highlighted(const Context& ctx) const;

protected:
    // Set by Root and FocusManager
    friend class Root;
    friend class FocusManager;

    bool _hovered = false;
    bool _pressed = false;
    bool _focused = false;
    bool _captured = false;

private:
    static inline int _id_counter { 1 };

    static int generate_id()
    {
        return _id_counter++;
    }

    WidgetId _id;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_WIDGET_H
