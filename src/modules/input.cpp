#include "./asw/modules/input.h"

#include <unordered_map>
#include <vector>

#include "./asw/modules/action.h"
#include "./asw/modules/log.h"

namespace {
/// @brief Data structure for controller state, including button states and axis values.
using ControllerState = struct ControllerState {
    std::array<bool, asw::input::NUM_CONTROLLER_BUTTONS> pressed { false };
    std::array<bool, asw::input::NUM_CONTROLLER_BUTTONS> released { false };
    std::array<bool, asw::input::NUM_CONTROLLER_BUTTONS> down { false };

    bool any_pressed { false };
    int last_pressed { -1 };
    float dead_zone { 0.25F };

    std::array<float, asw::input::NUM_CONTROLLER_AXES> axis { 0 };

    SDL_Gamepad* gamepad { nullptr };
    std::string name;
};

/// @brief Active cursor stores the current active cursor. It is updated by
/// the core.
std::array<SDL_Cursor*, asw::input::NUM_CURSORS> cursors { nullptr };

/// @brief Global controller state.
std::vector<ControllerState> controller {};

/// @brief Map of SDL_JoystickID to controller index in the controller vector.
std::unordered_map<SDL_JoystickID, uint32_t> controller_id_map {};

asw::input::KeyState keyboard {};
asw::input::MouseState mouse {};
std::string text_input;
} // namespace

const asw::input::KeyState& asw::input::get_keyboard()
{
    return keyboard;
}

const asw::input::MouseState& asw::input::get_mouse()
{
    return mouse;
}

const std::string& asw::input::get_text_input()
{
    return text_input;
}

void asw::input::_append_text(const char* text)
{
    text_input += text;
}

void asw::input::reset()
{
    // Snapshot action states before raw input arrays are cleared.
    asw::input::update_actions();

    auto& k_state = keyboard;
    auto& m_state = mouse;

    // Clear key state
    k_state.any_pressed = false;
    k_state.last_pressed = -1;

    for (auto& pressed : k_state.pressed) {
        pressed = false;
    }

    for (auto& released : k_state.released) {
        released = false;
    }

    // Clear mouse state
    m_state.any_pressed = false;
    m_state.change = { 0.0F, 0.0F };
    m_state.z = 0;

    for (auto& pressed : m_state.pressed) {
        pressed = false;
    }

    for (auto& released : m_state.released) {
        released = false;
    }

    // Clear text input
    text_input.clear();

    // Clear controller state
    for (auto& cont : controller) {
        cont.any_pressed = false;
        cont.last_pressed = -1;

        for (auto& button : cont.pressed) {
            button = false;
        }

        for (auto& button : cont.released) {
            button = false;
        }
    }
}

// ---- MOUSE ----
//

bool asw::input::get_mouse_button(asw::input::MouseButton button)
{
    return mouse.down[static_cast<int>(button)];
}

bool asw::input::get_mouse_button_down(asw::input::MouseButton button)
{
    return mouse.pressed[static_cast<int>(button)];
}

bool asw::input::get_mouse_button_up(asw::input::MouseButton button)
{
    return mouse.released[static_cast<int>(button)];
}

void asw::input::set_cursor(asw::input::CursorId cursor)
{
    auto cursor_int = static_cast<uint32_t>(cursor);

    if (cursor_int >= cursors.size()) {
        return;
    }

    if (cursors[cursor_int] == nullptr) {
        cursors[cursor_int] = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor_int));
    }

    SDL_SetCursor(cursors[cursor_int]);
}

// ---- KEYBOARD ----
//

bool asw::input::get_key(asw::input::Key key)
{
    return keyboard.down[static_cast<int>(key)];
}

bool asw::input::get_key_down(asw::input::Key key)
{
    return keyboard.pressed[static_cast<int>(key)];
}

bool asw::input::get_key_up(asw::input::Key key)
{
    return keyboard.released[static_cast<int>(key)];
}

// ---- CONTROLLER ----
//

bool asw::input::get_controller_button(uint32_t index, asw::input::ControllerButton button)
{
    if (index >= controller.size()) {
        return false;
    }

    return controller[index].down[static_cast<int>(button)];
}

bool asw::input::get_controller_button_down(uint32_t index, asw::input::ControllerButton button)
{
    if (index >= controller.size()) {
        return false;
    }

    return controller[index].pressed[static_cast<int>(button)];
}

bool asw::input::get_controller_button_up(uint32_t index, asw::input::ControllerButton button)
{
    if (index >= controller.size()) {
        return false;
    }

    return controller[index].released[static_cast<int>(button)];
}

float asw::input::get_controller_axis(uint32_t index, asw::input::ControllerAxis axis)
{
    if (index >= controller.size()) {
        return 0.0F;
    }

    return controller[index].axis[static_cast<int>(axis)];
}

void asw::input::set_controller_dead_zone(uint32_t index, float dead_zone)
{
    if (index >= controller.size()) {
        return;
    }

    controller[index].dead_zone = dead_zone;
}

int asw::input::get_controller_count()
{
    return controller.size();
}

std::string asw::input::get_controller_name(uint32_t index)
{
    if (index >= controller.size()) {
        return "";
    }

    return controller.at(index).name;
}

/// Event Hooks

void asw::input::_key_down(SDL_Scancode scancode)
{
    keyboard.pressed[scancode] = true;
    keyboard.down[scancode] = true;
    keyboard.any_pressed = true;
    keyboard.last_pressed = scancode;
}

void asw::input::_key_up(SDL_Scancode scancode)
{
    keyboard.released[scancode] = true;
    keyboard.down[scancode] = false;
}

void asw::input::_mouse_button_down(uint8_t button)
{
    const auto button_int = static_cast<int>(button);
    mouse.pressed[button_int] = true;
    mouse.down[button_int] = true;
    mouse.any_pressed = true;
    mouse.last_pressed = button_int;
}

void asw::input::_mouse_button_up(uint8_t button)
{
    const auto button_int = static_cast<int>(button);
    mouse.released[button_int] = true;
    mouse.down[button_int] = false;
}

void asw::input::_mouse_motion(float x, float y, float delta_x, float delta_y)
{
    mouse.position.x = x;
    mouse.position.y = y;
    mouse.change.x = delta_x;
    mouse.change.y = delta_y;
}

void asw::input::_mouse_wheel(float delta_z)
{
    mouse.z = delta_z;
}

void asw::input::_controller_added(SDL_JoystickID id)
{
    if (!SDL_IsGamepad(id)) {
        asw::log::warn("Failed to open gamepad: {}", id);
        return;
    }

    auto* opened = SDL_OpenGamepad(id);
    if (opened == nullptr) {
        asw::log::warn("Failed to open gamepad: {}", id);
    }

    // Add controller
    auto& new_controller = controller.emplace_back();
    new_controller.gamepad = opened;
    new_controller.name = SDL_GetGamepadName(opened);
    controller_id_map[id] = controller.size() - 1;

    asw::log::info("Gamepad added: {} (ID: {})", new_controller.name, id);
}

void asw::input::_controller_removed(SDL_JoystickID id)
{
    const auto it = controller_id_map.find(id);
    if (it == controller_id_map.end()) {
        return;
    }

    // Erase from vector and map
    const auto index = it->second;
    controller_id_map.erase(it);
    controller.erase(controller.begin() + index);

    // Close gamepad if it exists
    if (auto* existing = SDL_GetGamepadFromID(id); existing != nullptr) {
        SDL_CloseGamepad(existing);
    }
}

void asw::input::_controller_axis_motion(SDL_JoystickID id, uint32_t axis, float value)
{
    const auto it = controller_id_map.find(id);
    if (it == controller_id_map.end()) {
        return;
    }

    const auto index = it->second;
    if (index >= controller.size() || axis >= asw::input::NUM_CONTROLLER_AXES) {
        return;
    }

    controller[index].axis[axis] = value / 32768.0F; // Normalize to [-1, 1]
}

void asw::input::_controller_button_down(SDL_JoystickID id, uint32_t button)
{
    const auto it = controller_id_map.find(id);
    if (it == controller_id_map.end()) {
        return;
    }

    const auto index = it->second;
    if (index >= controller.size()) {
        return;
    }

    controller[index].pressed[button] = true;
    controller[index].down[button] = true;
    controller[index].any_pressed = true;
    controller[index].last_pressed = button;
}

void asw::input::_controller_button_up(SDL_JoystickID id, uint32_t button)
{
    const auto it = controller_id_map.find(id);
    if (it == controller_id_map.end()) {
        return;
    }

    const auto index = it->second;
    if (index >= controller.size()) {
        return;
    }

    controller[index].released[button] = true;
    controller[index].down[button] = false;
}