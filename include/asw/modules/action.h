/// @file action.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Action binding system for the ASW library
/// @date 2026-03-14
///
/// @copyright Copyright (c) 2026
///
/// Allows registering named actions backed by one or more mouse, keyboard or
/// controller bindings. Any binding that is active will satisfy the action.
///
/// Example:
/// @code
///   asw::input::bind_action("jump", asw::input::KeyBinding{asw::input::Key::Space});
///   asw::input::bind_action("jump",
///   asw::input::ControllerButtonBinding{asw::input::ControllerButton::A});
///
///   // In game loop:
///   if (asw::input::is_action_down("jump")) { /* ... */ }
/// @endcode

#ifndef ASW_ACTION_H
#define ASW_ACTION_H

#include <string>
#include <string_view>
#include <variant>

#include "./input.h"

namespace asw::input {

/// @brief Binding to a keyboard key.
struct KeyBinding {
    Key key;
};

/// @brief Binding to a mouse button.
struct MouseButtonBinding {
    MouseButton button;
};

/// @brief Binding to a controller (gamepad) button.
struct ControllerButtonBinding {
    ControllerButton button;
    uint32_t controller_index { 0 };
};

/// @brief Binding to a controller axis, activated when the axis exceeds a threshold.
///
/// @details Set @c positive_direction to false to bind to the negative axis direction
/// (e.g. left stick left, or left trigger in inverted mode).
struct ControllerAxisBinding {
    ControllerAxis axis;
    uint32_t controller_index { 0 };
    float threshold { 0.5F };
    bool positive_direction { true };
};

/// @brief A single input binding — keyboard, mouse button, controller button, or controller axis.
using ActionBinding
    = std::variant<KeyBinding, MouseButtonBinding, ControllerButtonBinding, ControllerAxisBinding>;

/// @brief Register a binding for a named action.
///
/// Multiple bindings can be added to the same action; any active binding will
/// satisfy the action (logical OR).
///
/// @param name    The action name.
/// @param binding The input binding to associate with the action.
///
void bind_action(std::string_view name, ActionBinding binding);

/// @brief Remove all bindings for a named action.
///
/// @param name The action name to remove.
///
void unbind_action(std::string_view name);

/// @brief Remove all registered actions and their bindings.
///
void clear_actions();

/// @brief Check if an action was triggered (first pressed) this frame.
///
/// @param name The action name.
/// @return true if any binding transitioned to active this frame.
///
bool is_action_down(std::string_view name);

/// @brief Check if an action was released this frame.
///
/// @param name The action name.
/// @return true if any binding transitioned to inactive this frame.
///
bool is_action_up(std::string_view name);

/// @brief Check if an action is currently held down.
///
/// @param name The action name.
/// @return true if any binding is currently active.
///
bool is_action(std::string_view name);

/// @brief Get the analogue strength of an action (0.0 – 1.0).
///
/// For button/key bindings this is 0 or 1. For axis bindings it is the
/// normalised axis value clamped to the range [threshold, 1.0].
/// Returns the maximum strength across all active bindings.
///
/// @param name The action name.
/// @return float Strength in [0.0, 1.0].
///
float get_action_strength(std::string_view name);

/// @brief Update cached action states from current raw input.
///
/// Called automatically by asw::input::reset() — you do not need to call
/// this yourself unless you are managing the input loop manually.
///
void update_actions();

} // namespace asw::input

#endif // ASW_ACTION_H
