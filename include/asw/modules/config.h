/// @file config.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Settings from a config file, set by whoever runs the game
/// @date 2026-09-30
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_CONFIG_H
#define ASW_CONFIG_H

#include <optional>
#include <string>

namespace asw::config {

/// @brief Name of the environment variable that holds the config file path.
inline constexpr const char* PATH_VARIABLE = "ASW_CONFIG";

/// @brief Load the file named by ASW_CONFIG, if it is set. Called by
/// asw::core::init() before the window opens.
///
void _init();

/// @brief Remove all settings. Called by asw::core::shutdown().
///
void _shutdown();

/// @brief Load settings from a file. Settings already loaded stay, unless
/// the file sets them again.
///
/// @details Each line is `key = value`. Blank lines and lines starting with
/// `#` or `;` are ignored. Keys are grouped with dots, such as
/// `display.fullscreen`.
///
/// @param path The file to load.
/// @return true if the file was read.
///
bool load(const std::string& path);

/// @brief Check if a setting is set.
///
/// @param key The setting, such as "display.fullscreen".
/// @return true if it is set.
///
bool has(const std::string& key);

/// @brief Get a setting as text.
///
/// @param key The setting.
/// @return The value, or nothing if it is not set.
///
std::optional<std::string> get_string(const std::string& key);

/// @brief Get a setting as true or false. 1, true, yes and on are true. 0,
/// false, no and off are false. Case does not matter.
///
/// @param key The setting.
/// @return The value, or nothing if it is not set or is not one of those.
///
std::optional<bool> get_bool(const std::string& key);

/// @brief Get a setting as a whole number.
///
/// @param key The setting.
/// @return The value, or nothing if it is not set or is not a whole number.
///
std::optional<int> get_int(const std::string& key);

/// @brief Get a setting as a number.
///
/// @param key The setting.
/// @return The value, or nothing if it is not set or is not a number.
///
std::optional<float> get_float(const std::string& key);

} // namespace asw::config

#endif // ASW_CONFIG_H
