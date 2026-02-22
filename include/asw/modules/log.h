/// @file log.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Structured logging system
/// @date 2026-02-13
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_LOG_H
#define ASW_LOG_H

#include <format>
#include <iostream>
#include <string>

namespace asw::log {

/// @brief Log severity levels
enum class Level { DEBUG, INFO, WARN, ERROR };

/// @brief Log a message at the specified level. Messages below the current log level are ignored.
///
/// @param level The severity level of the message.
/// @param message The message to log.
///
void log_message(Level level, const std::string& message);

/// @brief Set the minimum log level (messages below this are ignored).
///
/// @param level The minimum level to log.
///
void set_level(Level level);

/// @brief Set the output stream (default: std::cerr).
///
/// @param stream The output stream to write log messages to.
///
void set_output(std::ostream& stream);

/// @brief Log a debug message.
///
/// @param message The message to log.
///
void debug(const std::string& message);

/// @brief Log a formatted debug message.
///
/// @param format The format string.
/// @param args The arguments to format.
///
template <typename... Args> void debug(std::format_string<Args...> format, Args&&... args)
{
    log_message(Level::DEBUG, std::format(format, std::forward<Args>(args)...));
}

/// @brief Log an info message.
///
/// @param message The message to log.
///
void info(const std::string& message);

/// @brief Log a formatted info message.
///
/// @param format The format string.
/// @param args The arguments to format.
///
template <typename... Args> void info(std::format_string<Args...> format, Args&&... args)
{
    log_message(Level::INFO, std::format(format, std::forward<Args>(args)...));
}

/// @brief Log a warning message.
///
/// @param message The message to log.
///
void warn(const std::string& message);

/// @brief Log a formatted warning message.
///
///
/// @param format The format string.
/// @param args The arguments to format.
///
template <typename... Args> void warn(std::format_string<Args...> format, Args&&... args)
{
    log_message(Level::WARN, std::format(format, std::forward<Args>(args)...));
}

/// @brief Log an error message.
///
/// @param message The message to log.
///
void error(const std::string& message);

/// @brief Log a formatted error message.
///
/// @param format The format string.
/// @param args The arguments to format.
///
template <typename... Args> void error(std::format_string<Args...> format, Args&&... args)
{
    log_message(Level::ERROR, std::format(format, std::forward<Args>(args)...));
}

} // namespace asw::log

#endif // ASW_LOG_H
