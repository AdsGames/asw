#include "./asw/modules/log.h"

#include <chrono>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {
  asw::log::Level currentLevel = asw::log::Level::INFO;
  std::ostream* output = &std::cerr;

  const char* levelToString(asw::log::Level level) {
    switch (level) {
      case asw::log::Level::DEBUG:
        return "DEBUG";
      case asw::log::Level::INFO:
        return "INFO ";
      case asw::log::Level::WARN:
        return "WARN ";
      case asw::log::Level::ERROR:
        return "ERROR";
    }
    return "?????";
  }

  std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
  }

  void logMessage(asw::log::Level level, const std::string& message) {
    if (level < currentLevel) {
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
    emscripten_log(emLevel, "[%s] [%s] %s", levelToString(level),
                   getTimestamp().c_str(), message.c_str());
#else
    *output << "[" << levelToString(level) << "] "
            << "[" << getTimestamp() << "] " << message << "\n";
#endif
  }
}  // namespace

void asw::log::setLevel(Level level) {
  currentLevel = level;
}

void asw::log::setOutput(std::ostream& stream) {
  output = &stream;
}

void asw::log::debug(const std::string& message) {
  logMessage(Level::DEBUG, message);
}

void asw::log::info(const std::string& message) {
  logMessage(Level::INFO, message);
}

void asw::log::warn(const std::string& message) {
  logMessage(Level::WARN, message);
}

void asw::log::error(const std::string& message) {
  logMessage(Level::ERROR, message);
}
