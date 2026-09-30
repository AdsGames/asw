/// @file modal.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Modal widget for the ASW UI module
/// @date 2026-09-29
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_MODAL_H
#define ASW_MODULES_UI_MODAL_H

#include <functional>
#include <optional>
#include <string>

#include "button.h"
#include "label.h"
#include "layout.h"
#include "theme.h"

namespace asw::ui {

/// @brief A box over the rest of the UI that has to be dealt with first, e.g.
/// a question or a login code.
///
/// @details Open one with Root::open_modal. While it is open the screen
/// behind is dimmed, the pointer and focus only reach the modal, and back
/// closes it. Its children stack top to bottom, centred, and it sizes itself
/// to them.
///
class Modal : public Stack {
public:
    /// @brief Default constructor.
    ///
    Modal();

    /// @brief Called once when the modal closes, from close() or back.
    std::function<void()> on_close;

    /// @brief Close back when pressed. Turn off for a modal the player must
    /// answer with one of its buttons.
    bool close_on_back = true;

    /// @brief Style for this modal only. Uses the theme modal style when empty.
    std::optional<ModalStyle> style;

    /// @brief Get the style this modal draws with.
    ///
    /// @param ctx The UI context.
    /// @return The modal's own style, or the theme modal style.
    ///
    const ModalStyle& get_style(const Context& ctx) const;

    /// @brief Add a line of text, sized to fit.
    ///
    /// @param text The text.
    /// @param font The font, the theme font when empty.
    /// @return The label, to change its colour or justify.
    ///
    Label& add_text(const std::string& text, const asw::Font& font = nullptr);

    /// @brief Add a button, sized to its text.
    ///
    /// @param text The button text.
    /// @param on_click Called when it is clicked.
    /// @return The button.
    ///
    Button& add_button(const std::string& text, std::function<void()> on_click);

    /// @brief Close the modal. Calls on_close, then takes it off the screen.
    /// Safe to call from its own buttons or from on_close.
    ///
    void close();

    /// @brief True once closed.
    ///
    /// @return true - If close() was called.
    ///
    bool closed() const
    {
        return _closed;
    }

    /// @brief Size to the children, at least the style's min_width.
    ///
    /// @param ctx The UI context.
    ///
    void measure(Context& ctx) override;

    /// @brief Close on back.
    ///
    /// @param ctx The UI context.
    /// @param e The event.
    /// @return True if the event was handled.
    ///
    bool on_event(Context& ctx, const UIEvent& e) override;

    /// @brief Draw the box, then the children.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

private:
    bool _closed = false;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_MODAL_H
