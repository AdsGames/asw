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

namespace asw::input {
  /// Type defs
  /// @brief Number of mouse buttons in the ASW library.
  constexpr int ASW_NUM_MOUSE_BUTTONS = 5;

  /// @brief Number of keys in the ASW library.
  constexpr int ASW_NUM_KEYS = SDL_SCANCODE_COUNT;

  /// @brief Enumeration of mouse buttons in the ASW library.
  enum class MouseButton {
    LEFT = SDL_BUTTON_LEFT,
    MIDDLE = SDL_BUTTON_MIDDLE,
    RIGHT = SDL_BUTTON_RIGHT,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2,
    MAX = ASW_NUM_MOUSE_BUTTONS
  };

  /// @brief Enumeration of keys in the ASW library.
  enum class Key {
    UNKNOWN = SDL_SCANCODE_UNKNOWN,
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
    NUM_1 = SDL_SCANCODE_1,
    NUM_2 = SDL_SCANCODE_2,
    NUM_3 = SDL_SCANCODE_3,
    NUM_4 = SDL_SCANCODE_4,
    NUM_5 = SDL_SCANCODE_5,
    NUM_6 = SDL_SCANCODE_6,
    NUM_7 = SDL_SCANCODE_7,
    NUM_8 = SDL_SCANCODE_8,
    NUM_9 = SDL_SCANCODE_9,
    NUM_0 = SDL_SCANCODE_0,
    RETURN = SDL_SCANCODE_RETURN,
    ESCAPE = SDL_SCANCODE_ESCAPE,
    BACKSPACE = SDL_SCANCODE_BACKSPACE,
    TAB = SDL_SCANCODE_TAB,
    SPACE = SDL_SCANCODE_SPACE,
    MINUS = SDL_SCANCODE_MINUS,
    EQUALS = SDL_SCANCODE_EQUALS,
    LEFT_BRACKET = SDL_SCANCODE_LEFTBRACKET,
    RIGHT_BRACKET = SDL_SCANCODE_RIGHTBRACKET,
    BACKSLASH = SDL_SCANCODE_BACKSLASH,
    NONUSHASH = SDL_SCANCODE_NONUSHASH,
    SEMICOLON = SDL_SCANCODE_SEMICOLON,
    APOSTROPHE = SDL_SCANCODE_APOSTROPHE,
    GRAVE = SDL_SCANCODE_GRAVE,
    COMMA = SDL_SCANCODE_COMMA,
    PERIOD = SDL_SCANCODE_PERIOD,
    SLASH = SDL_SCANCODE_SLASH,
    CAPSLOCK = SDL_SCANCODE_CAPSLOCK,
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
    PRINT_SCREEN = SDL_SCANCODE_PRINTSCREEN,
    SCROL_LLOCK = SDL_SCANCODE_SCROLLLOCK,
    PAUSE = SDL_SCANCODE_PAUSE,
    INSERT = SDL_SCANCODE_INSERT,
    HOME = SDL_SCANCODE_HOME,
    PAGE_UP = SDL_SCANCODE_PAGEUP,
    DELETE = SDL_SCANCODE_DELETE,
    END = SDL_SCANCODE_END,
    PAGE_DOWN = SDL_SCANCODE_PAGEDOWN,
    RIGHT = SDL_SCANCODE_RIGHT,
    LEFT = SDL_SCANCODE_LEFT,
    DOWN = SDL_SCANCODE_DOWN,
    UP = SDL_SCANCODE_UP,
    NUM_LOCK_CLEAR = SDL_SCANCODE_NUMLOCKCLEAR,
    KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE,
    KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY,
    KP_MINUS = SDL_SCANCODE_KP_MINUS,
    KP_PLUS = SDL_SCANCODE_KP_PLUS,
    KP_ENTER = SDL_SCANCODE_KP_ENTER,
    KP_1 = SDL_SCANCODE_KP_1,
    KP_2 = SDL_SCANCODE_KP_2,
    KP_3 = SDL_SCANCODE_KP_3,
    KP_4 = SDL_SCANCODE_KP_4,
    KP_5 = SDL_SCANCODE_KP_5,
    KP_6 = SDL_SCANCODE_KP_6,
    KP_7 = SDL_SCANCODE_KP_7,
    KP_8 = SDL_SCANCODE_KP_8,
    KP_9 = SDL_SCANCODE_KP_9,
    KP_0 = SDL_SCANCODE_KP_0,
    KP_PERIOD = SDL_SCANCODE_KP_PERIOD,
    NONUS_BACKSLASH = SDL_SCANCODE_NONUSBACKSLASH,
    APPLICATION = SDL_SCANCODE_APPLICATION,
    POWER = SDL_SCANCODE_POWER,
    KP_EQUALS = SDL_SCANCODE_KP_EQUALS,
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
    EXECUTE = SDL_SCANCODE_EXECUTE,
    HELP = SDL_SCANCODE_HELP,
    MENU = SDL_SCANCODE_MENU,
    SELECT = SDL_SCANCODE_SELECT,
    STOP = SDL_SCANCODE_STOP,
    AGAIN = SDL_SCANCODE_AGAIN,
    UNDO = SDL_SCANCODE_UNDO,
    CUT = SDL_SCANCODE_CUT,
    COPY = SDL_SCANCODE_COPY,
    PASTE = SDL_SCANCODE_PASTE,
    FIND = SDL_SCANCODE_FIND,
    MUTE = SDL_SCANCODE_MUTE,
    VOLUME_UP = SDL_SCANCODE_VOLUMEUP,
    VOLUME_DOWN = SDL_SCANCODE_VOLUMEDOWN,
    KP_COMMA = SDL_SCANCODE_KP_COMMA,
    KP_EQUALSAS_400 = SDL_SCANCODE_KP_EQUALSAS400,
    INTERNATIONAL_1 = SDL_SCANCODE_INTERNATIONAL1,
    INTERNATIONAL_2 = SDL_SCANCODE_INTERNATIONAL2,
    INTERNATIONAL_3 = SDL_SCANCODE_INTERNATIONAL3,
    INTERNATIONAL_4 = SDL_SCANCODE_INTERNATIONAL4,
    INTERNATIONAL_5 = SDL_SCANCODE_INTERNATIONAL5,
    INTERNATIONAL_6 = SDL_SCANCODE_INTERNATIONAL6,
    INTERNATIONAL_7 = SDL_SCANCODE_INTERNATIONAL7,
    INTERNATIONAL_8 = SDL_SCANCODE_INTERNATIONAL8,
    INTERNATIONAL_9 = SDL_SCANCODE_INTERNATIONAL9,
    LANG_1 = SDL_SCANCODE_LANG1,
    LANG_2 = SDL_SCANCODE_LANG2,
    LANG_3 = SDL_SCANCODE_LANG3,
    LANG_4 = SDL_SCANCODE_LANG4,
    LANG_5 = SDL_SCANCODE_LANG5,
    LANG_6 = SDL_SCANCODE_LANG6,
    LANG_7 = SDL_SCANCODE_LANG7,
    LANG_8 = SDL_SCANCODE_LANG8,
    LANG_9 = SDL_SCANCODE_LANG9,
    ALT_ERASE = SDL_SCANCODE_ALTERASE,
    SYS_REQ = SDL_SCANCODE_SYSREQ,
    CANCEL = SDL_SCANCODE_CANCEL,
    CLEAR = SDL_SCANCODE_CLEAR,
    PRIOR = SDL_SCANCODE_PRIOR,
    RETURN_2 = SDL_SCANCODE_RETURN2,
    SEPARATOR = SDL_SCANCODE_SEPARATOR,
    OUT = SDL_SCANCODE_OUT,
    OPER = SDL_SCANCODE_OPER,
    CLEAR_AGAIN = SDL_SCANCODE_CLEARAGAIN,
    CRSEL = SDL_SCANCODE_CRSEL,
    EXSEL = SDL_SCANCODE_EXSEL,
    KP_00 = SDL_SCANCODE_KP_00,
    KP_000 = SDL_SCANCODE_KP_000,
    THOUSANDS_SEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR,
    DECIMAL_SEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR,
    CURRENCY_UNIT = SDL_SCANCODE_CURRENCYUNIT,
    CURRENCY_SUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT,
    KP_LEFT_PAREN = SDL_SCANCODE_KP_LEFTPAREN,
    KP_RIGHT_PAREN = SDL_SCANCODE_KP_RIGHTPAREN,
    KP_LEFT_BRACE = SDL_SCANCODE_KP_LEFTBRACE,
    KP_RIGHT_BRACE = SDL_SCANCODE_KP_RIGHTBRACE,
    KP_TAB = SDL_SCANCODE_KP_TAB,
    KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE,
    KP_A = SDL_SCANCODE_KP_A,
    KP_B = SDL_SCANCODE_KP_B,
    KP_C = SDL_SCANCODE_KP_C,
    KP_D = SDL_SCANCODE_KP_D,
    KP_E = SDL_SCANCODE_KP_E,
    KP_F = SDL_SCANCODE_KP_F,
    KP_XOR = SDL_SCANCODE_KP_XOR,
    KP_POWER = SDL_SCANCODE_KP_POWER,
    KP_PERCENT = SDL_SCANCODE_KP_PERCENT,
    KP_LESS = SDL_SCANCODE_KP_LESS,
    KP_GREATER = SDL_SCANCODE_KP_GREATER,
    KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND,
    KP_DOUBLE_AMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND,
    KP_VERTICAL_BAR = SDL_SCANCODE_KP_VERTICALBAR,
    KP_DOUBLE_VERTICAL_BAR = SDL_SCANCODE_KP_DBLVERTICALBAR,
    KP_COLON = SDL_SCANCODE_KP_COLON,
    KP_HASH = SDL_SCANCODE_KP_HASH,
    KP_SPACE = SDL_SCANCODE_KP_SPACE,
    KP_AT = SDL_SCANCODE_KP_AT,
    KP_EXCLAMATION = SDL_SCANCODE_KP_EXCLAM,
    KP_MEM_STORE = SDL_SCANCODE_KP_MEMSTORE,
    KP_MEM_RECALL = SDL_SCANCODE_KP_MEMRECALL,
    KP_MEM_CLEAR = SDL_SCANCODE_KP_MEMCLEAR,
    KP_MEM_ADD = SDL_SCANCODE_KP_MEMADD,
    KP_MEM_SUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT,
    KP_MEM_MULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY,
    KP_MEM_DIVIDE = SDL_SCANCODE_KP_MEMDIVIDE,
    KP_PLUS_MINUS = SDL_SCANCODE_KP_PLUSMINUS,
    KP_CLEAR = SDL_SCANCODE_KP_CLEAR,
    KP_CLEAR_ENTRY = SDL_SCANCODE_KP_CLEARENTRY,
    KP_BINARY = SDL_SCANCODE_KP_BINARY,
    KP_OCTAL = SDL_SCANCODE_KP_OCTAL,
    KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL,
    KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL,
    LCTRL = SDL_SCANCODE_LCTRL,
    LSHIFT = SDL_SCANCODE_LSHIFT,
    LALT = SDL_SCANCODE_LALT,
    LGUI = SDL_SCANCODE_LGUI,
    RCTRL = SDL_SCANCODE_RCTRL,
    RSHIFT = SDL_SCANCODE_RSHIFT,
    RALT = SDL_SCANCODE_RALT,
    RGUI = SDL_SCANCODE_RGUI,
    MODE = SDL_SCANCODE_MODE,
    AUDIO_NEXT = SDL_SCANCODE_MEDIA_NEXT_TRACK,
    AUDIO_PREV = SDL_SCANCODE_MEDIA_PREVIOUS_TRACK,
    AUDIO_STOP = SDL_SCANCODE_MEDIA_STOP,
    AUDIO_PLAY = SDL_SCANCODE_MEDIA_PLAY,
    MEDIA_SELECT = SDL_SCANCODE_MEDIA_SELECT,
    AC_SEARCH = SDL_SCANCODE_AC_SEARCH,
    AC_HOME = SDL_SCANCODE_AC_HOME,
    AC_BACK = SDL_SCANCODE_AC_BACK,
    AC_FORWARD = SDL_SCANCODE_AC_FORWARD,
    AC_STOP = SDL_SCANCODE_AC_STOP,
    AC_REFRESH = SDL_SCANCODE_AC_REFRESH,
    AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS,
    EJECT = SDL_SCANCODE_MEDIA_EJECT,
    SLEEP = SDL_SCANCODE_SLEEP,
    AUDIO_REWIND = SDL_SCANCODE_MEDIA_REWIND,
    AUDIO_FAST_FORWARD = SDL_SCANCODE_MEDIA_FAST_FORWARD,
    SOFT_LEFT = SDL_SCANCODE_SOFTLEFT,
    SOFT_RIGHT = SDL_SCANCODE_SOFTRIGHT,
    CALL = SDL_SCANCODE_CALL,
    ENDCALL = SDL_SCANCODE_ENDCALL,
    NUM_SCANCODES = ASW_NUM_KEYS
  };

  /// @brief Cursor count
  constexpr int ASW_NUM_CURSORS = SDL_SYSTEM_CURSOR_COUNT;

  /// @brief System cursor Ids
  enum class CursorId {
    DEFAULT = SDL_SYSTEM_CURSOR_DEFAULT,
    TEXT = SDL_SYSTEM_CURSOR_TEXT,
    WAIT = SDL_SYSTEM_CURSOR_WAIT,
    CROSSHAIR = SDL_SYSTEM_CURSOR_CROSSHAIR,
    PROGRESS = SDL_SYSTEM_CURSOR_PROGRESS,
    NWSE_RESIZE = SDL_SYSTEM_CURSOR_NWSE_RESIZE,
    NESW_RESIZE = SDL_SYSTEM_CURSOR_NESW_RESIZE,
    EW_RESIZE = SDL_SYSTEM_CURSOR_EW_RESIZE,
    NS_RESIZE = SDL_SYSTEM_CURSOR_NS_RESIZE,
    MOVE = SDL_SYSTEM_CURSOR_MOVE,
    NOT_ALLOWED = SDL_SYSTEM_CURSOR_NOT_ALLOWED,
    POINTER = SDL_SYSTEM_CURSOR_POINTER,
    NW_RESIZE = SDL_SYSTEM_CURSOR_NW_RESIZE,
    N_RESIZE = SDL_SYSTEM_CURSOR_N_RESIZE,
    NE_RESIZE = SDL_SYSTEM_CURSOR_NE_RESIZE,
    E_RESIZE = SDL_SYSTEM_CURSOR_E_RESIZE,
    SE_RESIZE = SDL_SYSTEM_CURSOR_SE_RESIZE,
    S_RESIZE = SDL_SYSTEM_CURSOR_S_RESIZE,
    SW_RESIZE = SDL_SYSTEM_CURSOR_SW_RESIZE,
    W_RESIZE = SDL_SYSTEM_CURSOR_W_RESIZE,
    COUNT = ASW_NUM_CURSORS
  };

  namespace {
    /// @brief Active cursor stores the current active cursor. It is updated by
    /// the core.
    std::array<SDL_Cursor*, ASW_NUM_CURSORS> cursors{nullptr};
  }  // namespace

  /// @brief Reset the input state.
  void reset();

  /// @brief Mouse state stores the current state of the mouse. It is updated by
  /// the core.
  using MouseState = struct MouseState {
    bool anyPressed{false};
    int lastPressed{-1};

    float xChange{0};
    float yChange{0};

    float x{0};
    float y{0};
    float z{0};

    std::array<bool, ASW_NUM_MOUSE_BUTTONS> pressed{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> released{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> down{false};
  };

  /// @brief Global mouse state.
  extern MouseState mouse;

  /// @brief Check if a button is down.
  ///
  /// @param button The button to check.
  /// @return true - If the button is down.
  /// @return false - If the button is not down.
  ///
  bool isButtonDown(asw::input::MouseButton button);

  /// @brief Check if a button was pressed since the last update.
  ///
  /// @param button The button to check.
  /// @return true - If the button was pressed.
  /// @return false - If the button was not pressed.
  ///
  bool wasButtonPressed(asw::input::MouseButton button);

  /// @brief Check if a button was released since the last update.
  ///
  /// @param button The button to check.
  /// @return true - If the button was released.
  /// @return false - If the button was not released.
  ///
  bool wasButtonReleased(asw::input::MouseButton button);

  /// @brief Keyboard state stores the current state of the keyboard. It is
  /// updated by the core.
  ///
  using KeyState = struct KeyState {
    std::array<bool, ASW_NUM_KEYS> pressed{false};
    std::array<bool, ASW_NUM_KEYS> released{false};
    std::array<bool, ASW_NUM_KEYS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};
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
  bool isKeyDown(asw::input::Key key);

  /// @brief Check if a key was pressed since the last update.
  ///
  /// @param key The key to check.
  /// @return true - If the key was pressed.
  /// @return false - If the key was not pressed.
  ///
  bool wasKeyPressed(asw::input::Key key);

  /// @brief Check if a key was released since the last update.
  ///
  /// @param key The key to check.
  /// @return true - If the key was released.
  /// @return false - If the key was not released.
  ///
  bool wasKeyReleased(asw::input::Key key);

  /// @brief Change cursor
  ///
  /// @param cursor The cursor to change to.
  /// @param window The window to change the cursor for.
  /// @return true - If the cursor was changed.
  /// @return false - If the cursor was not changed.
  ///
  void setCursor(asw::input::CursorId cursor);
}  // namespace asw::input

#endif  // ASW_INPUT_H