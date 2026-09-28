/// @file navigation.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Action based navigation for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_NAVIGATION_H
#define ASW_MODULES_UI_NAVIGATION_H

#include <string>
#include <string_view>

namespace asw::ui {

/// @brief Action names the UI reads for focus navigation, see asw/modules/action.h.
///
/// @details An empty name uses the built in keys for that step: arrows, Tab
/// and Shift+Tab, Return or Space, and Escape. Set a name to read that action
/// instead, so the UI follows the game's own bindings, controllers included.
///
/// Example:
/// @code
///   // Default keyboard and controller bindings
///   ui.ctx.navigation = asw::ui::bind_default_navigation();
///
///   // Or the game's own actions
///   ui.ctx.navigation.up = "move_up";
///   ui.ctx.navigation.activate = "jump";
/// @endcode
///
struct Navigation {
    /// @brief Move focus up.
    std::string up;

    /// @brief Move focus down.
    std::string down;

    /// @brief Move focus left.
    std::string left;

    /// @brief Move focus right.
    std::string right;

    /// @brief Move focus to the next widget. Holding Shift makes it go back,
    /// so Shift+Tab still works when Tab is bound here.
    std::string next;

    /// @brief Move focus to the previous widget.
    std::string prev;

    /// @brief Press the focused widget.
    std::string activate;

    /// @brief Go back, sent to the focused widget and then Root::on_back.
    std::string back;
};

/// @brief Bind keyboard and controller actions for UI navigation and return
/// their names. Calling it again replaces the bindings.
///
/// @details Binds, for any controller:
///   - up, down, left, right: arrow keys, D-pad and left stick
///   - next: Tab and right shoulder, prev: left shoulder
///   - activate: Return, Space and A
///   - back: Escape and B
///
/// @param prefix Prefix for the action names, e.g. "ui_" gives "ui_up".
/// @return The action names, to set on Context::navigation.
///
Navigation bind_default_navigation(std::string_view prefix = "ui_");

} // namespace asw::ui

#endif // ASW_MODULES_UI_NAVIGATION_H
