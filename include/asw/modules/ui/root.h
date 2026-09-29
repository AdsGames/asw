/// @file root.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Root UI container and event dispatcher for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_UI_ROOT_H
#define ASW_UI_ROOT_H

#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "context.h"
#include "modal.h"
#include "panel.h"

namespace asw::ui {

/// @brief A short message shown at the top of the screen, see Root::toast.
///
struct Toast {
    /// @brief The message.
    std::string text;

    /// @brief Picture before the text, e.g. an achievement icon. Optional.
    asw::Texture icon;

    /// @brief How long it stays, the theme toast seconds when 0.
    float seconds = 0.0F;

    /// @brief Text colour, the theme toast text colour when empty.
    std::optional<asw::Color> color;
};

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

    /// @brief Show a message at the top of the screen for a few seconds, e.g.
    /// "Achievement unlocked". Messages queue when several arrive together.
    ///
    /// @param text The message.
    ///
    void toast(const std::string& text);

    /// @brief Show a message with an icon, colour or length of its own.
    ///
    /// @param message The message.
    ///
    void toast(Toast message);

    /// @brief Remove every message, shown or waiting.
    ///
    void clear_toasts();

    /// @brief Messages shown or waiting.
    ///
    /// @return The number of messages.
    ///
    std::size_t toast_count() const
    {
        return _toasts.size();
    }

    /// @brief Open a modal over the rest of the UI. Its first focusable
    /// widget takes focus, and focus goes back where it was once every modal
    /// closes.
    ///
    /// @tparam T Modal or a class derived from it.
    /// @param args Arguments for the modal's constructor.
    /// @return The modal, add its content with add_text and add_button.
    ///
    template <class T = Modal, class... Args> T& open_modal(Args&&... args)
    {
        auto& modal = _modals.add_child<T>(std::forward<Args>(args)...);
        opened_modal();
        return modal;
    }

    /// @brief True while a modal is open.
    ///
    /// @return true - If a modal is open.
    ///
    bool has_modal() const;

    /// @brief Close every open modal, top first.
    ///
    void close_modals();

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

    // The top open modal, or nullptr
    Modal* top_modal() const;

    // Where the pointer and focus go: the top modal, or the root panel
    Widget& active_root();

    void opened_modal();
    void layout_modals();
    void update_modal_focus();
    void update_toasts();
    void draw_toasts();

    bool _used = false;

    // Open modals, drawn over root. See through and screen sized.
    Panel _modals;

    // Focus to give back once the modals close, and the modal whose first
    // widget was focused
    Widget* _focus_before_modal = nullptr;
    Widget* _focused_modal = nullptr;

    struct ShownToast {
        Toast toast;
        float age = 0.0F;
    };
    std::deque<ShownToast> _toasts;
    std::optional<std::chrono::steady_clock::time_point> _last_update;
};

} // namespace asw::ui

#endif // ASW_UI_ROOT_H
