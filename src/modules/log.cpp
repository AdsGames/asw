#include "./asw/modules/log.h"

#include <array>
#include <chrono>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {
asw::log::Level current_level = asw::log::Level::Info;
std::ostream* output = &std::cerr;

const char* level_to_string(asw::log::Level level)
{
    using enum asw::log::Level;

    switch (level) {
    case Debug:
        return "DEBUG";
    case Info:
        return "INFO ";
    case Warn:
        return "WARN ";
    case Error:
        return "ERROR";
    }
    return "?????";
}

/// @brief ANSI escape codes for terminal formatting
enum class AnsiCode : int {
    Reset = 0,
    BoldOn = 1,
    Faint = 2,
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

    // Standard text colors
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
    BgDefault = 49,

    // Bright (high-intensity) text colors
    TextBrightBlack = 90,
    TextBrightRed = 91,
    TextBrightGreen = 92,
    TextBrightYellow = 93,
    TextBrightBlue = 94,
    TextBrightMagenta = 95,
    TextBrightCyan = 96,
    TextBrightWhite = 97,
};

inline std::string ansi_to_string(AnsiCode code)
{
    return "\033[" + std::to_string(static_cast<int>(code)) + "m";
}

AnsiCode level_to_ansi(asw::log::Level level)
{
    using enum asw::log::Level;

    switch (level) {
    case Debug:
        return AnsiCode::TextBrightCyan;
    case Info:
        return AnsiCode::TextBrightWhite;
    case Warn:
        return AnsiCode::TextBrightYellow;
    case Error:
        return AnsiCode::TextBrightRed;
    }
    return AnsiCode::Reset;
}

// Local wall clock time with milliseconds. Formatting a system_clock time
// point directly prints UTC.
std::string get_timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
    const auto millis
        = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
        % 1000;

    std::tm local {};
#ifdef _WIN32
    localtime_s(&local, &seconds);
#else
    localtime_r(&seconds, &local);
#endif

    std::array<char, 16> clock {};
    std::strftime(clock.data(), clock.size(), "%H:%M:%S", &local);
    return std::format("{}.{:03}", clock.data(), millis);
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
    case asw::log::Level::Debug:
        emLevel = EM_LOG_CONSOLE;
        break;
    case asw::log::Level::Info:
        emLevel = EM_LOG_CONSOLE;
        break;
    case asw::log::Level::Warn:
        emLevel = EM_LOG_WARN;
        break;
    case asw::log::Level::Error:
        emLevel = EM_LOG_ERROR;
        break;
    }
    emscripten_log(
        emLevel, "[%s] [%s] %s", level_to_string(level), get_timestamp().c_str(), message.c_str());
#else
    const std::string reset = ansi_to_string(AnsiCode::Reset);
    const std::string color = ansi_to_string(level_to_ansi(level));
    const std::string bold = ansi_to_string(AnsiCode::BoldOn);
    const std::string faint = ansi_to_string(AnsiCode::Faint);

    // Dim timestamp
    *output << faint << "[" << get_timestamp() << "]" << reset << " ";

    // Bold + bright colored level badge
    *output << bold << color << "[" << level_to_string(level) << "]" << reset << " ";

    // Level-colored message
    *output << color << message << reset << "\n";
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

void asw::log::debug(const std::string& message)
{
    log_message(Level::Debug, message);
}

void asw::log::info(const std::string& message)
{
    log_message(Level::Info, message);
}

void asw::log::warn(const std::string& message)
{
    log_message(Level::Warn, message);
}

void asw::log::error(const std::string& message)
{
    log_message(Level::Error, message);
}

void asw::log::progress(float progress, std::string message)
{
    constexpr int bar_width = 20;
    const int filled = static_cast<int>(progress * static_cast<float>(bar_width));

    // Build Unicode block bar: █ filled, ░ empty
    std::string bar;
    bar.reserve(bar_width * 3);
    for (int i = 0; i < bar_width; ++i) {
        bar += (i < filled) ? "█" : "░";
    }

    const std::string progress_message
        = std::format("[{}] {:>3}% {}", bar, static_cast<int>(progress * 100.0f), message);
    log_message(Level::Info, progress_message);
}
