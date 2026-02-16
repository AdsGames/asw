/// @file log.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Structured logging system
/// @date 2026-02-13
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_LOG_H
#define ASW_LOG_H

#include <iostream>
#include <string>

namespace asw::log {

  /// @brief Log severity levels
  enum class Level { DEBUG, INFO, WARN, ERROR };

  /// @brief Set the minimum log level (messages below this are ignored).
  ///
  /// @param level The minimum level to log.
  ///
  void setLevel(Level level);

  /// @brief Set the output stream (default: std::cerr).
  ///
  /// @param stream The output stream to write log messages to.
  ///
  void setOutput(std::ostream& stream);

  /// @brief Log a debug message.
  ///
  /// @param message The message to log.
  ///
  void debug(const std::string& message);

  /// @brief Log an info message.
  ///
  /// @param message The message to log.
  ///
  void info(const std::string& message);

  /// @brief Log a warning message.
  ///
  /// @param message The message to log.
  ///
  void warn(const std::string& message);

  /// @brief Log an error message.
  ///
  /// @param message The message to log.
  ///
  void error(const std::string& message);

}  // namespace asw::log

#endif  // ASW_LOG_H
