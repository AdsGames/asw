#include "./asw/modules/log.h"

#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {
asw::log::Level current_level = asw::log::Level::INFO;
std::ostream* output = &std::cerr;

const char* level_to_string(asw::log::Level level)
{
    using enum asw::log::Level;

    switch (level) {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO ";
    case WARN:
        return "WARN ";
    case ERROR:
        return "ERROR";
    }
    return "?????";
}

/// @brief ANSI escape codes for terminal formatting
enum class AnsiCode : int {
    Reset = 0,
    BoldOn = 1,
    FaintOff = 2,
    ItalicOn = 3,
    UnderlineOn = 4,
    SlowBlinkOn = 5,
    RapidBlinkOn = 6,
    ReverseVideoOn = 7,
    ConcealOn = 8,
    CrossedOutOn = 9,
    BoldOff = 22,
    ItalicOff = 23,
    UnderlineOff = 24,
    BlinkOff = 25,
    ReverseVideoOff = 27,
    ConcealOff = 28,
    CrossedOutOff = 29,

    // Text color
    TextBlack = 30,
    TextRed = 31,
    TextGreen = 32,
    TextYellow = 33,
    TextBlue = 34,
    TextMagenta = 35,
    TextCyan = 36,
    TextWhite = 37,
    TextDefault = 39,

    // Background color
    BgBlack = 40,
    BgRed = 41,
    BgGreen = 42,
    BgYellow = 43,
    BgBlue = 44,
    BgMagenta = 45,
    BgCyan = 46,
    BgWhite = 47,
    BgDefault = 49
};

inline std::string ansi_to_string(AnsiCode code)
{
    return "\033[" + std::to_string(static_cast<int>(code)) + "m";
}

AnsiCode level_to_ansi(asw::log::Level level)
{
    using enum asw::log::Level;

    switch (level) {
    case DEBUG:
        return AnsiCode::TextCyan;
    case INFO:
        return AnsiCode::TextGreen;
    case WARN:
        return AnsiCode::TextYellow;
    case ERROR:
        return AnsiCode::TextRed;
    }
    return AnsiCode::Reset;
}

std::string get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    return std::format("{:%H:%M:%S}", now);
}
} // namespace

void asw::log::log_message(asw::log::Level level, const std::string& message)
{
    if (level < current_level) {
        return;
    }

#ifdef __EMSCRIPTEN__
    int emLevel = EM_LOG_CONSOLE;
    switch (level) {
    case asw::log::Level::DEBUG:
        emLevel = EM_LOG_CONSOLE;
        break;
    case asw::log::Level::INFO:
        emLevel = EM_LOG_CONSOLE;
        break;
    case asw::log::Level::WARN:
        emLevel = EM_LOG_WARN;
        break;
    case asw::log::Level::ERROR:
        emLevel = EM_LOG_ERROR;
        break;
    }
    emscripten_log(
        emLevel, "[%s] [%s] %s", level_to_string(level), get_timestamp().c_str(), message.c_str());
#else
    *output << ansi_to_string(level_to_ansi(level));
    *output << "[" << level_to_string(level) << "] ";
    *output << "[" << get_timestamp() << "] ";
    *output << message;
    *output << "\n";
    *output << ansi_to_string(AnsiCode::Reset);
#endif
}

void asw::log::set_level(Level level)
{
    current_level = level;
}

void asw::log::set_output(std::ostream& stream)
{
    output = &stream;
}
