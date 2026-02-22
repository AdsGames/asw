/// @file event.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief UI event types for the ASW UI module
/// @date 2026-02-21
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_EVENT_H
#define ASW_MODULES_UI_EVENT_H

#include <string>

#include "../geometry.h"
#include "../input.h"

namespace asw::ui {

/// @brief Event structure for UI interactions.
///
struct UIEvent {
    /// @brief Enumeration of UI event types.
    enum class Type {
        KeyDown,
        KeyUp,
        TextInput,
        PointerDown,
        PointerUp,
        PointerMove,
        PointerEnter,
        PointerLeave,
        Activate,
        Back
    };

    /// @brief The type of the event.
    Type type {};

    /// @brief The key associated with the event.
    asw::input::Key key {};

    /// @brief Whether the shift key is held.
    bool shift = false;

    /// @brief The pointer identifier.
    int pointer_id = 0;

    /// @brief The pointer position.
    asw::Vec2<float> pointer_pos;

    /// @brief The mouse button associated with the event.
    asw::input::MouseButton mouse_button {};

    /// @brief The text associated with a TextInput event.
    std::string text;
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_EVENT_H
