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

std::string get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    return std::format("{:%H:%M:%S}", now);
}

void log_message(asw::log::Level level, const std::string& message)
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
    *output << "[" << level_to_string(level) << "] "
            << "[" << get_timestamp() << "] " << message << "\n";
#endif
}
} // namespace

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
    log_message(Level::DEBUG, message);
}

void asw::log::info(const std::string& message)
{
    log_message(Level::INFO, message);
}

void asw::log::warn(const std::string& message)
{
    log_message(Level::WARN, message);
}

void asw::log::error(const std::string& message)
{
    log_message(Level::ERROR, message);
}
