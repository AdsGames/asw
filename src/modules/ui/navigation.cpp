#include "./asw/modules/ui/navigation.h"

#include <initializer_list>

#include "./asw/modules/action.h"

namespace {
using namespace asw::input;

std::string bind(
    std::string_view prefix, std::string_view name, std::initializer_list<ActionBinding> bindings)
{
    std::string action { prefix };
    action += name;

    unbind_action(action);
    for (const auto& binding : bindings) {
        bind_action(action, binding);
    }
    return action;
}

ControllerButtonBinding pad(ControllerButton button)
{
    return { .button = button, .controller_index = ANY_CONTROLLER };
}

ControllerAxisBinding stick(ControllerAxis axis, bool positive)
{
    return { .axis = axis,
        .controller_index = ANY_CONTROLLER,
        .threshold = 0.5F,
        .positive_direction = positive };
}
} // namespace

asw::ui::Navigation asw::ui::bind_default_navigation(std::string_view prefix)
{
    Navigation nav;

    nav.up = bind(prefix, "up",
        { KeyBinding { Key::Up }, pad(ControllerButton::DPadUp),
            stick(ControllerAxis::LeftY, false) });
    nav.down = bind(prefix, "down",
        { KeyBinding { Key::Down }, pad(ControllerButton::DPadDown),
            stick(ControllerAxis::LeftY, true) });
    nav.left = bind(prefix, "left",
        { KeyBinding { Key::Left }, pad(ControllerButton::DPadLeft),
            stick(ControllerAxis::LeftX, false) });
    nav.right = bind(prefix, "right",
        { KeyBinding { Key::Right }, pad(ControllerButton::DPadRight),
            stick(ControllerAxis::LeftX, true) });
    nav.next
        = bind(prefix, "next", { KeyBinding { Key::Tab }, pad(ControllerButton::RightShoulder) });
    nav.prev = bind(prefix, "prev", { pad(ControllerButton::LeftShoulder) });
    nav.activate = bind(prefix, "activate",
        { KeyBinding { Key::Return }, KeyBinding { Key::Space }, pad(ControllerButton::A) });
    nav.back = bind(prefix, "back", { KeyBinding { Key::Escape }, pad(ControllerButton::B) });

    return nav;
}
