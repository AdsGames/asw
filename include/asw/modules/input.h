/// @file input.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Input module for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_INPUT_H
#define ASW_INPUT_H

#include <SDL3/SDL.h>
#include <array>
#include <string>

#include "./geometry.h"

namespace asw::input {
  /// Type defs
  /// @brief Number of mouse buttons in the ASW library.
  constexpr int NUM_MOUSE_BUTTONS = 6;

  /// @brief Number of keys in the ASW library.
  constexpr int NUM_KEYS = SDL_SCANCODE_COUNT;

  /// @brief Enumeration of mouse buttons in the ASW library.
  enum class MouseButton {
    Left = SDL_BUTTON_LEFT,
    Middle = SDL_BUTTON_MIDDLE,
    Right = SDL_BUTTON_RIGHT,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2,
    NumMouseButtons = NUM_MOUSE_BUTTONS
  };

  /// @brief Enumeration of keys in the ASW library.
  enum class Key {
    Unknown = SDL_SCANCODE_UNKNOWN,
    A = SDL_SCANCODE_A,
    B = SDL_SCANCODE_B,
    C = SDL_SCANCODE_C,
    D = SDL_SCANCODE_D,
    E = SDL_SCANCODE_E,
    F = SDL_SCANCODE_F,
    G = SDL_SCANCODE_G,
    H = SDL_SCANCODE_H,
    I = SDL_SCANCODE_I,
    J = SDL_SCANCODE_J,
    K = SDL_SCANCODE_K,
    L = SDL_SCANCODE_L,
    M = SDL_SCANCODE_M,
    N = SDL_SCANCODE_N,
    O = SDL_SCANCODE_O,
    P = SDL_SCANCODE_P,
    Q = SDL_SCANCODE_Q,
    R = SDL_SCANCODE_R,
    S = SDL_SCANCODE_S,
    T = SDL_SCANCODE_T,
    U = SDL_SCANCODE_U,
    V = SDL_SCANCODE_V,
    W = SDL_SCANCODE_W,
    X = SDL_SCANCODE_X,
    Y = SDL_SCANCODE_Y,
    Z = SDL_SCANCODE_Z,
    Num1 = SDL_SCANCODE_1,
    Num2 = SDL_SCANCODE_2,
    Num3 = SDL_SCANCODE_3,
    Num4 = SDL_SCANCODE_4,
    Num5 = SDL_SCANCODE_5,
    Num6 = SDL_SCANCODE_6,
    Num7 = SDL_SCANCODE_7,
    Num8 = SDL_SCANCODE_8,
    Num9 = SDL_SCANCODE_9,
    Num0 = SDL_SCANCODE_0,
    Return = SDL_SCANCODE_RETURN,
    Escape = SDL_SCANCODE_ESCAPE,
    Backspace = SDL_SCANCODE_BACKSPACE,
    Tab = SDL_SCANCODE_TAB,
    Space = SDL_SCANCODE_SPACE,
    Minus = SDL_SCANCODE_MINUS,
    Equals = SDL_SCANCODE_EQUALS,
    LeftBracket = SDL_SCANCODE_LEFTBRACKET,
    RightBracket = SDL_SCANCODE_RIGHTBRACKET,
    Backslash = SDL_SCANCODE_BACKSLASH,
    NonUsHash = SDL_SCANCODE_NONUSHASH,
    Semicolon = SDL_SCANCODE_SEMICOLON,
    Apostrophe = SDL_SCANCODE_APOSTROPHE,
    Grave = SDL_SCANCODE_GRAVE,
    Comma = SDL_SCANCODE_COMMA,
    Period = SDL_SCANCODE_PERIOD,
    Slash = SDL_SCANCODE_SLASH,
    Capslock = SDL_SCANCODE_CAPSLOCK,
    F1 = SDL_SCANCODE_F1,
    F2 = SDL_SCANCODE_F2,
    F3 = SDL_SCANCODE_F3,
    F4 = SDL_SCANCODE_F4,
    F5 = SDL_SCANCODE_F5,
    F6 = SDL_SCANCODE_F6,
    F7 = SDL_SCANCODE_F7,
    F8 = SDL_SCANCODE_F8,
    F9 = SDL_SCANCODE_F9,
    F10 = SDL_SCANCODE_F10,
    F11 = SDL_SCANCODE_F11,
    F12 = SDL_SCANCODE_F12,
    PrintScreen = SDL_SCANCODE_PRINTSCREEN,
    ScrollLock = SDL_SCANCODE_SCROLLLOCK,
    Pause = SDL_SCANCODE_PAUSE,
    Insert = SDL_SCANCODE_INSERT,
    Home = SDL_SCANCODE_HOME,
    PageUp = SDL_SCANCODE_PAGEUP,
    Delete = SDL_SCANCODE_DELETE,
    End = SDL_SCANCODE_END,
    PageDown = SDL_SCANCODE_PAGEDOWN,
    Right = SDL_SCANCODE_RIGHT,
    Left = SDL_SCANCODE_LEFT,
    Down = SDL_SCANCODE_DOWN,
    Up = SDL_SCANCODE_UP,
    NumLockClear = SDL_SCANCODE_NUMLOCKCLEAR,
    KpDivide = SDL_SCANCODE_KP_DIVIDE,
    KpMultiply = SDL_SCANCODE_KP_MULTIPLY,
    KpMinus = SDL_SCANCODE_KP_MINUS,
    KpPlus = SDL_SCANCODE_KP_PLUS,
    KpEnter = SDL_SCANCODE_KP_ENTER,
    Kp1 = SDL_SCANCODE_KP_1,
    Kp2 = SDL_SCANCODE_KP_2,
    Kp3 = SDL_SCANCODE_KP_3,
    Kp4 = SDL_SCANCODE_KP_4,
    Kp5 = SDL_SCANCODE_KP_5,
    Kp6 = SDL_SCANCODE_KP_6,
    Kp7 = SDL_SCANCODE_KP_7,
    Kp8 = SDL_SCANCODE_KP_8,
    Kp9 = SDL_SCANCODE_KP_9,
    Kp0 = SDL_SCANCODE_KP_0,
    KpPeriod = SDL_SCANCODE_KP_PERIOD,
    NonusBackslash = SDL_SCANCODE_NONUSBACKSLASH,
    Application = SDL_SCANCODE_APPLICATION,
    Power = SDL_SCANCODE_POWER,
    KpEquals = SDL_SCANCODE_KP_EQUALS,
    F13 = SDL_SCANCODE_F13,
    F14 = SDL_SCANCODE_F14,
    F15 = SDL_SCANCODE_F15,
    F16 = SDL_SCANCODE_F16,
    F17 = SDL_SCANCODE_F17,
    F18 = SDL_SCANCODE_F18,
    F19 = SDL_SCANCODE_F19,
    F20 = SDL_SCANCODE_F20,
    F21 = SDL_SCANCODE_F21,
    F22 = SDL_SCANCODE_F22,
    F23 = SDL_SCANCODE_F23,
    F24 = SDL_SCANCODE_F24,
    Execute = SDL_SCANCODE_EXECUTE,
    Help = SDL_SCANCODE_HELP,
    Menu = SDL_SCANCODE_MENU,
    Select = SDL_SCANCODE_SELECT,
    Stop = SDL_SCANCODE_STOP,
    Again = SDL_SCANCODE_AGAIN,
    Undo = SDL_SCANCODE_UNDO,
    Cut = SDL_SCANCODE_CUT,
    Copy = SDL_SCANCODE_COPY,
    Paste = SDL_SCANCODE_PASTE,
    Find = SDL_SCANCODE_FIND,
    Mute = SDL_SCANCODE_MUTE,
    VolumeUp = SDL_SCANCODE_VOLUMEUP,
    VolumeDown = SDL_SCANCODE_VOLUMEDOWN,
    KpComma = SDL_SCANCODE_KP_COMMA,
    KpEqualsSas400 = SDL_SCANCODE_KP_EQUALSAS400,
    International1 = SDL_SCANCODE_INTERNATIONAL1,
    International2 = SDL_SCANCODE_INTERNATIONAL2,
    International3 = SDL_SCANCODE_INTERNATIONAL3,
    International4 = SDL_SCANCODE_INTERNATIONAL4,
    International5 = SDL_SCANCODE_INTERNATIONAL5,
    International6 = SDL_SCANCODE_INTERNATIONAL6,
    International7 = SDL_SCANCODE_INTERNATIONAL7,
    International8 = SDL_SCANCODE_INTERNATIONAL8,
    International9 = SDL_SCANCODE_INTERNATIONAL9,
    Lang1 = SDL_SCANCODE_LANG1,
    Lang2 = SDL_SCANCODE_LANG2,
    Lang3 = SDL_SCANCODE_LANG3,
    Lang4 = SDL_SCANCODE_LANG4,
    Lang5 = SDL_SCANCODE_LANG5,
    Lang6 = SDL_SCANCODE_LANG6,
    Lang7 = SDL_SCANCODE_LANG7,
    Lang8 = SDL_SCANCODE_LANG8,
    Lang9 = SDL_SCANCODE_LANG9,
    AltErase = SDL_SCANCODE_ALTERASE,
    SysReq = SDL_SCANCODE_SYSREQ,
    Cancel = SDL_SCANCODE_CANCEL,
    Clear = SDL_SCANCODE_CLEAR,
    Prior = SDL_SCANCODE_PRIOR,
    Return2 = SDL_SCANCODE_RETURN2,
    Separator = SDL_SCANCODE_SEPARATOR,
    Out = SDL_SCANCODE_OUT,
    Oper = SDL_SCANCODE_OPER,
    ClearAgain = SDL_SCANCODE_CLEARAGAIN,
    Crsel = SDL_SCANCODE_CRSEL,
    Exsel = SDL_SCANCODE_EXSEL,
    Kp00 = SDL_SCANCODE_KP_00,
    Kp000 = SDL_SCANCODE_KP_000,
    ThousandsSeparator = SDL_SCANCODE_THOUSANDSSEPARATOR,
    DecimalSeparator = SDL_SCANCODE_DECIMALSEPARATOR,
    CurrencyUnit = SDL_SCANCODE_CURRENCYUNIT,
    CurrencySubunit = SDL_SCANCODE_CURRENCYSUBUNIT,
    KpLeftParen = SDL_SCANCODE_KP_LEFTPAREN,
    KpRightParan = SDL_SCANCODE_KP_RIGHTPAREN,
    KpLeftBrace = SDL_SCANCODE_KP_LEFTBRACE,
    KpRightBrace = SDL_SCANCODE_KP_RIGHTBRACE,
    KpTab = SDL_SCANCODE_KP_TAB,
    KpBackspace = SDL_SCANCODE_KP_BACKSPACE,
    KpA = SDL_SCANCODE_KP_A,
    KpB = SDL_SCANCODE_KP_B,
    KpC = SDL_SCANCODE_KP_C,
    KpD = SDL_SCANCODE_KP_D,
    KpE = SDL_SCANCODE_KP_E,
    KpF = SDL_SCANCODE_KP_F,
    KpXor = SDL_SCANCODE_KP_XOR,
    KpPower = SDL_SCANCODE_KP_POWER,
    KpPercent = SDL_SCANCODE_KP_PERCENT,
    KpLess = SDL_SCANCODE_KP_LESS,
    KpGreater = SDL_SCANCODE_KP_GREATER,
    KpAmpersand = SDL_SCANCODE_KP_AMPERSAND,
    KpDoubleAmpersand = SDL_SCANCODE_KP_DBLAMPERSAND,
    KpVerticalBar = SDL_SCANCODE_KP_VERTICALBAR,
    KpDoubleVerticalBar = SDL_SCANCODE_KP_DBLVERTICALBAR,
    KpColon = SDL_SCANCODE_KP_COLON,
    KpHash = SDL_SCANCODE_KP_HASH,
    KpSpace = SDL_SCANCODE_KP_SPACE,
    KpAt = SDL_SCANCODE_KP_AT,
    KpExclam = SDL_SCANCODE_KP_EXCLAM,
    KpMemStore = SDL_SCANCODE_KP_MEMSTORE,
    KpMemRecall = SDL_SCANCODE_KP_MEMRECALL,
    KpMemClear = SDL_SCANCODE_KP_MEMCLEAR,
    KpMemAdd = SDL_SCANCODE_KP_MEMADD,
    KpMemSubtract = SDL_SCANCODE_KP_MEMSUBTRACT,
    KpMemMultiply = SDL_SCANCODE_KP_MEMMULTIPLY,
    KpMemDivide = SDL_SCANCODE_KP_MEMDIVIDE,
    KpPlusMinus = SDL_SCANCODE_KP_PLUSMINUS,
    KpClear = SDL_SCANCODE_KP_CLEAR,
    KpClearEntry = SDL_SCANCODE_KP_CLEARENTRY,
    KpBinary = SDL_SCANCODE_KP_BINARY,
    KpOctal = SDL_SCANCODE_KP_OCTAL,
    KpDecimal = SDL_SCANCODE_KP_DECIMAL,
    KpHexadecimal = SDL_SCANCODE_KP_HEXADECIMAL,
    LCtrl = SDL_SCANCODE_LCTRL,
    LShift = SDL_SCANCODE_LSHIFT,
    LAlt = SDL_SCANCODE_LALT,
    LGui = SDL_SCANCODE_LGUI,
    RCtrl = SDL_SCANCODE_RCTRL,
    RShift = SDL_SCANCODE_RSHIFT,
    RAlt = SDL_SCANCODE_RALT,
    RGui = SDL_SCANCODE_RGUI,
    Mode = SDL_SCANCODE_MODE,
    MediaNextTrack = SDL_SCANCODE_MEDIA_NEXT_TRACK,
    MediaPreviousTrack = SDL_SCANCODE_MEDIA_PREVIOUS_TRACK,
    MediaStop = SDL_SCANCODE_MEDIA_STOP,
    MediaPlay = SDL_SCANCODE_MEDIA_PLAY,
    MediaSelect = SDL_SCANCODE_MEDIA_SELECT,
    AcSearch = SDL_SCANCODE_AC_SEARCH,
    AcHome = SDL_SCANCODE_AC_HOME,
    AcBack = SDL_SCANCODE_AC_BACK,
    AcForward = SDL_SCANCODE_AC_FORWARD,
    AcStop = SDL_SCANCODE_AC_STOP,
    AcRefresh = SDL_SCANCODE_AC_REFRESH,
    AcBookmarks = SDL_SCANCODE_AC_BOOKMARKS,
    Eject = SDL_SCANCODE_MEDIA_EJECT,
    Sleep = SDL_SCANCODE_SLEEP,
    AudioRewind = SDL_SCANCODE_MEDIA_REWIND,
    AudioFastForward = SDL_SCANCODE_MEDIA_FAST_FORWARD,
    SoftLeft = SDL_SCANCODE_SOFTLEFT,
    SoftRight = SDL_SCANCODE_SOFTRIGHT,
    Call = SDL_SCANCODE_CALL,
    EndCall = SDL_SCANCODE_ENDCALL,
    NumKeys = NUM_KEYS
  };

  /// @brief Cursor count
  constexpr int NUM_CURSORS = SDL_SYSTEM_CURSOR_COUNT;

  /// @brief System cursor Ids
  enum class CursorId {
    Default = SDL_SYSTEM_CURSOR_DEFAULT,
    Text = SDL_SYSTEM_CURSOR_TEXT,
    Wait = SDL_SYSTEM_CURSOR_WAIT,
    Crosshair = SDL_SYSTEM_CURSOR_CROSSHAIR,
    Progress = SDL_SYSTEM_CURSOR_PROGRESS,
    NwseResize = SDL_SYSTEM_CURSOR_NWSE_RESIZE,
    NeswResize = SDL_SYSTEM_CURSOR_NESW_RESIZE,
    EwResize = SDL_SYSTEM_CURSOR_EW_RESIZE,
    NsResize = SDL_SYSTEM_CURSOR_NS_RESIZE,
    Move = SDL_SYSTEM_CURSOR_MOVE,
    NotAllowed = SDL_SYSTEM_CURSOR_NOT_ALLOWED,
    Pointer = SDL_SYSTEM_CURSOR_POINTER,
    NwResize = SDL_SYSTEM_CURSOR_NW_RESIZE,
    NResize = SDL_SYSTEM_CURSOR_N_RESIZE,
    NeResize = SDL_SYSTEM_CURSOR_NE_RESIZE,
    EResize = SDL_SYSTEM_CURSOR_E_RESIZE,
    SEResize = SDL_SYSTEM_CURSOR_SE_RESIZE,
    SResize = SDL_SYSTEM_CURSOR_S_RESIZE,
    SWResize = SDL_SYSTEM_CURSOR_SW_RESIZE,
    WResize = SDL_SYSTEM_CURSOR_W_RESIZE,
    NumCursors = NUM_CURSORS
  };

  /// @brief Mouse state stores the current state of the mouse. It is updated by
  /// the core.
  using MouseState = struct MouseState {
    bool any_pressed{false};
    int last_pressed{-1};

    Vec2<float> change{0.0F, 0.0F};
    Vec2<float> position{0.0F, 0.0F};
    float z{0.0F};

    std::array<bool, NUM_MOUSE_BUTTONS> pressed{false};
    std::array<bool, NUM_MOUSE_BUTTONS> released{false};
    std::array<bool, NUM_MOUSE_BUTTONS> down{false};
  };

  /// @brief Global mouse state.
  extern MouseState mouse;

  /// @brief Check if a mouse button is down.
  ///
  /// @param button The button to check.
  /// @return true - If the button is down.
  /// @return false - If the button is not down.
  ///
  bool get_mouse_button(asw::input::MouseButton button);

  /// @brief Check if a mouse button was pressed since the last update.
  ///
  /// @param button The button to check.
  /// @return true - If the button was pressed.
  /// @return false - If the button was not pressed.
  ///
  bool get_mouse_button_down(asw::input::MouseButton button);

  /// @brief Check if a mouse button was released since the last update.
  ///
  /// @param button The button to check.
  /// @return true - If the button was released.
  /// @return false - If the button was not released.
  ///
  bool get_mouse_button_up(asw::input::MouseButton button);

  /// @brief Keyboard state stores the current state of the keyboard. It is
  /// updated by the core.
  ///
  using KeyState = struct KeyState {
    std::array<bool, NUM_KEYS> pressed{false};
    std::array<bool, NUM_KEYS> released{false};
    std::array<bool, NUM_KEYS> down{false};

    bool any_pressed{false};
    int last_pressed{-1};
  };

  /// @brief Global keyboard state.
  ///
  extern KeyState keyboard;

  /// @brief Check if a key is down.
  ///
  /// @param key The key to check.
  /// @return true - If the key is down.
  /// @return false - If the key is not down.
  ///
  bool get_key(asw::input::Key key);

  /// @brief Check if a key was pressed since the last update.
  ///
  /// @param key The key to check.
  /// @return true - If the key was pressed.
  /// @return false - If the key was not pressed.
  ///
  bool get_key_down(asw::input::Key key);

  /// @brief Check if a key was released since the last update.
  ///
  /// @param key The key to check.
  /// @return true - If the key was released.
  /// @return false - If the key was not released.
  ///
  bool get_key_up(asw::input::Key key);

  /// @brief Change cursor
  ///
  /// @param cursor The cursor to change to.
  /// @param window The window to change the cursor for.
  /// @return true - If the cursor was changed.
  /// @return false - If the cursor was not changed.
  ///
  void set_cursor(asw::input::CursorId cursor);

  /**
   * @brief Number of buttons on a game controller
   */
  constexpr int NUM_CONTROLLER_BUTTONS = SDL_GAMEPAD_BUTTON_COUNT;

  /**
   * @brief Mappings from SDL game controller buttons to ASW buttons
   */
  enum class ControllerButton {
    A = SDL_GAMEPAD_BUTTON_SOUTH,
    B = SDL_GAMEPAD_BUTTON_EAST,
    X = SDL_GAMEPAD_BUTTON_WEST,
    Y = SDL_GAMEPAD_BUTTON_NORTH,
    Back = SDL_GAMEPAD_BUTTON_BACK,
    Guide = SDL_GAMEPAD_BUTTON_GUIDE,
    Start = SDL_GAMEPAD_BUTTON_START,
    LeftStick = SDL_GAMEPAD_BUTTON_LEFT_STICK,
    RightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    LeftShoulder = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
    RightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    DPadUp = SDL_GAMEPAD_BUTTON_DPAD_UP,
    DPadDown = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
    DPadLeft = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
    DPadRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
    Misc1 = SDL_GAMEPAD_BUTTON_MISC1,
    RightPaddle1 = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,
    LeftPaddle1 = SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,
    RightPaddle2 = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,
    LeftPaddle2 = SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,
    TouchPad = SDL_GAMEPAD_BUTTON_TOUCHPAD,
    NumControllerButtons = NUM_CONTROLLER_BUTTONS
  };

  /**
   * @brief Number of axes on a game controller
   */
  constexpr int NUM_CONTROLLER_AXES = SDL_GAMEPAD_AXIS_COUNT;

  /**
   * @brief Mappings from SDL game controller axes to ASW axes
   */
  enum class ControllerAxis {
    LeftX = SDL_GAMEPAD_AXIS_LEFTX,
    LeftY = SDL_GAMEPAD_AXIS_LEFTY,
    RightX = SDL_GAMEPAD_AXIS_RIGHTX,
    RightY = SDL_GAMEPAD_AXIS_RIGHTY,
    LeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
    RightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
    NumControllerAxes = NUM_CONTROLLER_AXES
  };

  /**
   * @brief Controller state stores the current state of a controller. It is
   * updated by the core.
   */
  using ControllerState = struct ControllerState {
    std::array<bool, NUM_CONTROLLER_BUTTONS> pressed{false};
    std::array<bool, NUM_CONTROLLER_BUTTONS> released{false};
    std::array<bool, NUM_CONTROLLER_BUTTONS> down{false};

    bool any_pressed{false};
    int last_pressed{-1};
    float dead_zone{0.25F};

    std::array<float, NUM_CONTROLLER_AXES> axis{0};
  };

  /**
   * @brief Maximum number of controllers supported
   */
  constexpr int MAX_CONTROLLERS = 8;

  /**
   * @brief Global controller state.
   */
  extern std::array<ControllerState, MAX_CONTROLLERS> controller;

  /**
   * @brief Check if a controller button is down.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button is down.
   * @return false - If the button is not down.
   */
  bool get_controller_button(uint32_t index,
                             asw::input::ControllerButton button);

  /**
   * @brief Check if a controller button was pressed since the last update.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button was pressed.
   * @return false - If the button was not pressed.
   */
  bool get_controller_button_down(uint32_t index,
                                  asw::input::ControllerButton button);

  /**
   * @brief Check if a controller button was released since the last update.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button was released.
   * @return false - If the button was not released.
   */
  bool get_controller_button_up(uint32_t index,
                                asw::input::ControllerButton button);

  /**
   * @brief Get the value of a controller axis.
   *
   * @param index The index of the controller to check.
   * @param axis The axis to check.
   * @return float - The value of the axis between -1.0f and 1.0f.
   */
  float get_controller_axis(uint32_t index, asw::input::ControllerAxis axis);

  /**
   * @brief Set the joystick deadzone for a controller.
   */
  void set_controller_dead_zone(uint32_t index, float dead_zone);

  /**
   * @brief Get the number of controllers connected.
   */
  int get_controller_count();

  /**
   * @brief Get the name of a controller.
   */
  std::string get_controller_name(uint32_t index);

  /**
   * @brief Reset all input states. Called by the core.
   */
  void reset();
}  // namespace asw::input

#endif  // ASW_INPUT_H