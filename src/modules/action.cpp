#include "./asw/modules/action.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {

struct ActionData {
    std::vector<asw::input::ActionBinding> bindings;

    bool pressed { false };
    bool released { false };
    bool down { false };
    float strength { 0.0F };

    /// Tracks the down state from the previous frame to derive axis transitions.
    bool prev_down { false };
};

std::unordered_map<std::string, ActionData> action_map;

// ---------------------------------------------------------------------------
// Per-binding query helpers
// ---------------------------------------------------------------------------

/// Returns true if the binding is currently active, and updates out_strength.
bool binding_is_down(const asw::input::ActionBinding& binding, float& out_strength)
{
    return std::visit(
        [&out_strength](const auto& b) -> bool {
            using T = std::decay_t<decltype(b)>;

            if constexpr (std::is_same_v<T, asw::input::KeyBinding>) {
                if (asw::input::get_key(b.key)) {
                    out_strength = std::max(out_strength, 1.0F);
                    return true;
                }
                return false;

            } else if constexpr (std::is_same_v<T, asw::input::MouseButtonBinding>) {
                if (asw::input::get_mouse_button(b.button)) {
                    out_strength = std::max(out_strength, 1.0F);
                    return true;
                }
                return false;

            } else if constexpr (std::is_same_v<T, asw::input::ControllerButtonBinding>) {
                if (asw::input::get_controller_button(b.controller_index, b.button)) {
                    out_strength = std::max(out_strength, 1.0F);
                    return true;
                }
                return false;

            } else if constexpr (std::is_same_v<T, asw::input::ControllerAxisBinding>) {
                const float val = asw::input::get_controller_axis(b.controller_index, b.axis);
                const float effective = b.positive_direction ? val : -val;
                if (effective >= b.threshold) {
                    out_strength = std::max(out_strength, effective);
                    return true;
                }

                return false;
            }

            return false;
        },
        binding);
}

/// Returns true if the binding was pressed this frame (digital sources only).
/// Axis bindings return false here — their transitions are handled via prev_down.
bool binding_is_pressed(const asw::input::ActionBinding& binding)
{
    return std::visit(
        [](const auto& b) -> bool {
            using T = std::decay_t<decltype(b)>;

            if constexpr (std::is_same_v<T, asw::input::KeyBinding>) {
                return asw::input::get_key_down(b.key);

            } else if constexpr (std::is_same_v<T, asw::input::MouseButtonBinding>) {
                return asw::input::get_mouse_button_down(b.button);

            } else if constexpr (std::is_same_v<T, asw::input::ControllerButtonBinding>) {
                return asw::input::get_controller_button_down(b.controller_index, b.button);

            } else {
                // ControllerAxisBinding: transition handled by prev_down in update_actions.
                return false;
            }
        },
        binding);
}

/// Returns true if the binding was released this frame (digital sources only).
bool binding_is_released(const asw::input::ActionBinding& binding)
{
    return std::visit(
        [](const auto& b) -> bool {
            using T = std::decay_t<decltype(b)>;

            if constexpr (std::is_same_v<T, asw::input::KeyBinding>) {
                return asw::input::get_key_up(b.key);

            } else if constexpr (std::is_same_v<T, asw::input::MouseButtonBinding>) {
                return asw::input::get_mouse_button_up(b.button);

            } else if constexpr (std::is_same_v<T, asw::input::ControllerButtonBinding>) {
                return asw::input::get_controller_button_up(b.controller_index, b.button);

            } else {
                // ControllerAxisBinding: transition handled by prev_down in update_actions.
                return false;
            }
        },
        binding);
}

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void asw::input::bind_action(std::string_view name, asw::input::ActionBinding binding)
{
    action_map[std::string(name)].bindings.push_back(binding);
}

void asw::input::unbind_action(std::string_view name)
{
    action_map.erase(std::string(name));
}

void asw::input::clear_actions()
{
    action_map.clear();
}

bool asw::input::is_action_down(std::string_view name)
{
    auto it = action_map.find(std::string(name));
    return it != action_map.end() && it->second.pressed;
}

bool asw::input::is_action_up(std::string_view name)
{
    auto it = action_map.find(std::string(name));
    return it != action_map.end() && it->second.released;
}

bool asw::input::is_action(std::string_view name)
{
    auto it = action_map.find(std::string(name));
    return it != action_map.end() && it->second.down;
}

float asw::input::get_action_strength(std::string_view name)
{
    auto it = action_map.find(std::string(name));
    return it != action_map.end() ? it->second.strength : 0.0F;
}

void asw::input::update_actions()
{
    for (auto& [name, action] : action_map) {
        bool any_down = false;
        bool any_pressed = false;
        bool any_released = false;
        float max_strength = 0.0F;

        for (const auto& binding : action.bindings) {
            any_down |= binding_is_down(binding, max_strength);
            any_pressed |= binding_is_pressed(binding);
            any_released |= binding_is_released(binding);
        }

        // Derive press / release transitions for axis bindings (and as a
        // fallback for any binding that doesn't supply its own signals).
        if (any_down && !action.prev_down) {
            any_pressed = true;
        }
        if (!any_down && action.prev_down) {
            any_released = true;
        }

        action.pressed = any_pressed;
        action.released = any_released;
        action.down = any_down;
        action.strength = max_strength;
        action.prev_down = any_down;
    }
}
