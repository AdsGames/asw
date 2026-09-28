#include "./asw/modules/input.h"

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "./asw/modules/action.h"
#include "./asw/modules/display.h"
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
asw::input::InputDevice last_device { asw::input::InputDevice::KeyboardMouse };

/// @brief Rescale a value past the dead zone so it starts from zero.
float rescale_dead_zone(float magnitude, float dead_zone)
{
    if (magnitude <= dead_zone) {
        return 0.0F;
    }

    return std::min((magnitude - dead_zone) / (1.0F - dead_zone), 1.0F);
}

/// @brief Read a stick with a radial dead zone.
asw::Vec2<float> read_stick(const ControllerState& cont, asw::input::ControllerStick stick)
{
    const bool left = stick == asw::input::ControllerStick::Left;
    const auto x_axis
        = left ? asw::input::ControllerAxis::LeftX : asw::input::ControllerAxis::RightX;
    const auto y_axis
        = left ? asw::input::ControllerAxis::LeftY : asw::input::ControllerAxis::RightY;

    const asw::Vec2<float> raw(
        cont.axis[static_cast<int>(x_axis)], cont.axis[static_cast<int>(y_axis)]);
    const float magnitude = std::hypot(raw.x, raw.y);
    const float scaled = rescale_dead_zone(magnitude, cont.dead_zone);

    if (scaled == 0.0F) {
        return { 0.0F, 0.0F };
    }

    return raw * (scaled / magnitude);
}

/// @brief Read one axis with the dead zone applied.
float read_axis(const ControllerState& cont, asw::input::ControllerAxis axis)
{
    using asw::input::ControllerAxis;
    using asw::input::ControllerStick;

    switch (axis) {
    case ControllerAxis::LeftX:
        return read_stick(cont, ControllerStick::Left).x;
    case ControllerAxis::LeftY:
        return read_stick(cont, ControllerStick::Left).y;
    case ControllerAxis::RightX:
        return read_stick(cont, ControllerStick::Right).x;
    case ControllerAxis::RightY:
        return read_stick(cont, ControllerStick::Right).y;
    default: {
        const float value = cont.axis[static_cast<int>(axis)];
        return std::copysign(rescale_dead_zone(std::abs(value), cont.dead_zone), value);
    }
    }
}

/// @brief Check a button on one controller, or on any when index is ANY_CONTROLLER.
template <typename Pred> bool any_controller(uint32_t index, Pred pred)
{
    if (index == asw::input::ANY_CONTROLLER) {
        return std::any_of(controller.begin(), controller.end(), pred);
    }

    return index < controller.size() && pred(controller[index]);
}
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

void asw::input::_shutdown()
{
    for (auto& cursor : cursors) {
        if (cursor != nullptr) {
            SDL_DestroyCursor(cursor);
            cursor = nullptr;
        }
    }
}

void asw::input::reset()
{
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
    const auto index = static_cast<size_t>(button);
    return index < mouse.down.size() && mouse.down[index];
}

bool asw::input::get_mouse_button_down(asw::input::MouseButton button)
{
    const auto index = static_cast<size_t>(button);
    return index < mouse.pressed.size() && mouse.pressed[index];
}

bool asw::input::get_mouse_button_up(asw::input::MouseButton button)
{
    const auto index = static_cast<size_t>(button);
    return index < mouse.released.size() && mouse.released[index];
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

void asw::input::set_cursor_visible(bool visible)
{
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

namespace {
// Window the simulated event belongs to, so render scaling is applied to it
SDL_WindowID simulated_window_id()
{
    auto* window = asw::display::get_window();
    return window != nullptr ? SDL_GetWindowID(window) : 0;
}

// Each event struct is filled on its own and then assigned into the union, so
// the member written is the active one

void push_simulated_key(asw::input::Key key, bool down)
{
    SDL_KeyboardEvent key_event {};
    key_event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    key_event.windowID = simulated_window_id();
    key_event.scancode
        = static_cast<SDL_Scancode>(static_cast<std::underlying_type_t<asw::input::Key>>(key));
    key_event.down = down;

    SDL_Event e {};
    e.key = key_event;
    SDL_PushEvent(&e);
}

void push_simulated_mouse_button(asw::input::MouseButton button, bool down)
{
    SDL_MouseButtonEvent button_event {};
    button_event.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    button_event.windowID = simulated_window_id();
    button_event.button = static_cast<uint8_t>(
        static_cast<std::underlying_type_t<asw::input::MouseButton>>(button));
    button_event.down = down;
    button_event.clicks = 1;

    // SDL events carry window coordinates, the mouse position is in render
    // coordinates
    button_event.x = mouse.position.x;
    button_event.y = mouse.position.y;
    if (auto* renderer = asw::display::get_renderer(); renderer != nullptr) {
        SDL_RenderCoordinatesToWindow(
            renderer, mouse.position.x, mouse.position.y, &button_event.x, &button_event.y);
    }

    SDL_Event e {};
    e.button = button_event;
    SDL_PushEvent(&e);
}
} // namespace

void asw::input::simulate_key_down(asw::input::Key key)
{
    push_simulated_key(key, true);
}

void asw::input::simulate_key_up(asw::input::Key key)
{
    push_simulated_key(key, false);
}

void asw::input::simulate_mouse_move(const asw::Vec2<float>& position)
{
    // Core converts motion from window to render coordinates, so go the
    // other way here
    auto window_pos = position;
    auto window_prev = mouse.position;
    if (auto* renderer = asw::display::get_renderer(); renderer != nullptr) {
        SDL_RenderCoordinatesToWindow(
            renderer, position.x, position.y, &window_pos.x, &window_pos.y);
        SDL_RenderCoordinatesToWindow(
            renderer, mouse.position.x, mouse.position.y, &window_prev.x, &window_prev.y);
    }

    SDL_MouseMotionEvent motion {};
    motion.type = SDL_EVENT_MOUSE_MOTION;
    motion.windowID = simulated_window_id();
    motion.x = window_pos.x;
    motion.y = window_pos.y;
    motion.xrel = window_pos.x - window_prev.x;
    motion.yrel = window_pos.y - window_prev.y;

    SDL_Event e {};
    e.motion = motion;
    SDL_PushEvent(&e);
}

void asw::input::simulate_mouse_button_down(asw::input::MouseButton button)
{
    push_simulated_mouse_button(button, true);
}

void asw::input::simulate_mouse_button_up(asw::input::MouseButton button)
{
    push_simulated_mouse_button(button, false);
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
    const auto b = static_cast<int>(button);
    return any_controller(index, [b](const ControllerState& cont) { return cont.down[b]; });
}

bool asw::input::get_controller_button_down(uint32_t index, asw::input::ControllerButton button)
{
    const auto b = static_cast<int>(button);
    return any_controller(index, [b](const ControllerState& cont) { return cont.pressed[b]; });
}

bool asw::input::get_controller_button_up(uint32_t index, asw::input::ControllerButton button)
{
    const auto b = static_cast<int>(button);
    return any_controller(index, [b](const ControllerState& cont) { return cont.released[b]; });
}

float asw::input::get_controller_axis(uint32_t index, asw::input::ControllerAxis axis)
{
    if (index != ANY_CONTROLLER) {
        return index < controller.size() ? read_axis(controller[index], axis) : 0.0F;
    }

    float furthest = 0.0F;
    for (const auto& cont : controller) {
        const float value = read_axis(cont, axis);
        if (std::abs(value) > std::abs(furthest)) {
            furthest = value;
        }
    }

    return furthest;
}

asw::Vec2<float> asw::input::get_controller_stick(uint32_t index, asw::input::ControllerStick stick)
{
    if (index != ANY_CONTROLLER) {
        return index < controller.size() ? read_stick(controller[index], stick)
                                         : Vec2<float>(0.0F, 0.0F);
    }

    Vec2<float> furthest(0.0F, 0.0F);
    for (const auto& cont : controller) {
        const auto value = read_stick(cont, stick);
        if (std::hypot(value.x, value.y) > std::hypot(furthest.x, furthest.y)) {
            furthest = value;
        }
    }

    return furthest;
}

void asw::input::set_controller_dead_zone(uint32_t index, float dead_zone)
{
    // Below 1 so rescaling never divides by zero
    dead_zone = std::clamp(dead_zone, 0.0F, 0.99F);

    if (index == ANY_CONTROLLER) {
        for (auto& cont : controller) {
            cont.dead_zone = dead_zone;
        }
        return;
    }

    if (index >= controller.size()) {
        return;
    }

    controller[index].dead_zone = dead_zone;
}

asw::input::InputDevice asw::input::get_last_device()
{
    return last_device;
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
    last_device = asw::input::InputDevice::KeyboardMouse;
}

void asw::input::_key_up(SDL_Scancode scancode)
{
    keyboard.released[scancode] = true;
    keyboard.down[scancode] = false;
}

void asw::input::_mouse_button_down(uint8_t button)
{
    // Mice with many side buttons can report numbers past the ones we track
    if (button >= mouse.pressed.size()) {
        return;
    }

    const auto button_int = static_cast<int>(button);
    mouse.pressed[button_int] = true;
    mouse.down[button_int] = true;
    mouse.any_pressed = true;
    mouse.last_pressed = button_int;
    last_device = asw::input::InputDevice::KeyboardMouse;
}

void asw::input::_mouse_button_up(uint8_t button)
{
    if (button >= mouse.released.size()) {
        return;
    }

    const auto button_int = static_cast<int>(button);
    mouse.released[button_int] = true;
    mouse.down[button_int] = false;
}

void asw::input::_mouse_motion(float x, float y, float delta_x, float delta_y)
{
    mouse.position.x = x;
    mouse.position.y = y;
    // Several motion events can arrive in one frame, so add them up
    mouse.change.x += delta_x;
    mouse.change.y += delta_y;
    last_device = asw::input::InputDevice::KeyboardMouse;
}

void asw::input::_mouse_wheel(float delta_z)
{
    mouse.z += delta_z;
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
        return;
    }

    // Add controller
    auto& new_controller = controller.emplace_back();
    new_controller.gamepad = opened;
    const char* name = SDL_GetGamepadName(opened);
    new_controller.name = name != nullptr ? name : "";
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

    // Controllers after the removed one shift down a slot
    for (auto& [other_id, other_index] : controller_id_map) {
        if (other_index > index) {
            other_index--;
        }
    }

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

    if (read_axis(controller[index], static_cast<asw::input::ControllerAxis>(axis)) != 0.0F) {
        last_device = asw::input::InputDevice::Controller;
    }
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
    last_device = asw::input::InputDevice::Controller;
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