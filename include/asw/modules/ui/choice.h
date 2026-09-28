/// @file choice.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Choice widget for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_CHOICE_H
#define ASW_MODULES_UI_CHOICE_H

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "button.h"

namespace asw::ui {

/// @brief A button that cycles through a list of options, e.g. difficulty or
/// window mode.
///
/// @details Clicking or activating moves to the next option. While focused,
/// left and right move back and forward, unless the choice sits in a row
/// with other focusable widgets, where they move focus instead (see
/// adjust_on_left_right). Each option shows
/// its label from options, its texture from images, or both. The rest of the
/// look is the Button style.
///
class Choice : public Button {
public:
    /// @brief Option labels.
    std::vector<std::string> options;

    /// @brief Option textures, one per option. Optional.
    std::vector<asw::Texture> images;

    /// @brief Index of the selected option.
    std::size_t index = 0;

    /// @brief Wrap around at either end.
    bool wrap = true;

    /// @brief Whether left and right change the option. Empty to decide from
    /// the layout: off in a row with other focusable widgets, on otherwise.
    std::optional<bool> adjust_on_left_right;

    /// @brief Whether left and right change the option right now.
    ///
    /// @return adjust_on_left_right, or the layout default.
    ///
    bool adjusts_left_right() const;

    /// @brief Show < and > at the sides while highlighted, when there is text
    /// and left and right change the option.
    bool show_arrows = true;

    /// @brief Callback invoked with the new index when the user changes it.
    std::function<void(std::size_t)> on_change;

    /// @brief Number of options, the larger of options and images.
    ///
    /// @return The option count.
    ///
    std::size_t count() const;

    /// @brief Select an option by index without calling on_change.
    ///
    /// @param i The index, clamped to the options.
    ///
    void select(std::size_t i);

    /// @brief Move to the next option, calls on_change.
    ///
    void next();

    /// @brief Move to the previous option, calls on_change.
    ///
    void prev();

    /// @brief Move to the next option, then call on_click.
    ///
    /// @param ctx The UI context.
    ///
    void activate(Context& ctx) override;

    /// @brief Handle left and right while focused.
    ///
    /// @param ctx The UI context.
    /// @param e The event.
    /// @return True if the event was handled.
    ///
    bool on_event(Context& ctx, const UIEvent& e) override;

    /// @brief Draw the selected option.
    ///
    /// @param ctx The UI context.
    ///
    void draw(Context& ctx) override;

private:
    void step(int dir);
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_CHOICE_H
