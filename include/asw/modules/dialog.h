/// @file dialog.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Native message boxes and file choosers
/// @date 2026-09-27
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_DIALOG_H
#define ASW_DIALOG_H

#include <optional>
#include <string>
#include <vector>

namespace asw::dialog {

/// @brief Show an information message box. Blocks until it is closed.
///
/// @param title The title of the message box.
/// @param message The message to show.
///
void info(const std::string& title, const std::string& message);

/// @brief Show a warning message box. Blocks until it is closed.
///
/// @param title The title of the message box.
/// @param message The message to show.
///
void warn(const std::string& title, const std::string& message);

/// @brief Show an error message box. Blocks until it is closed.
///
/// @param title The title of the message box.
/// @param message The message to show.
///
void error(const std::string& title, const std::string& message);

/// @brief Ask a yes or no question. Blocks until it is answered.
///
/// @param title The title of the message box.
/// @param message The question to ask.
/// @return true - If yes was chosen.
/// @return false - If no was chosen, or the box was closed.
///
bool confirm(const std::string& title, const std::string& message);

/// @brief Kind of file chooser
enum class FileMode {
    Open,
    Save,
};

/// @brief Filter shown in a file chooser.
struct FileFilter {
    /// @brief Name shown to the user, for example "Levels".
    std::string name;

    /// @brief Semicolon separated extensions without dots, for example
    /// "xml;json", or "*" for all files.
    std::string pattern;
};

/// @brief Open a native file chooser. It does not block, the chosen file
/// arrives later through take_file(). Only one chooser can be open at a time.
///
/// @param mode Open an existing file, or choose where to save one.
/// @param default_location Folder or file the chooser starts at, empty for the
/// system default.
/// @param filters Filters to offer, empty for all files.
/// @return true - If the chooser was opened.
/// @return false - If a chooser is already open.
///
bool request_file(FileMode mode, const std::string& default_location = "",
    const std::vector<FileFilter>& filters = { });

/// @brief Check if a file chooser is open.
///
/// @return true - While waiting for the user to choose.
///
bool is_file_pending();

/// @brief Take the file the user chose. Returns it once, then empty until the
/// next chooser.
///
/// @return The chosen path, or empty while waiting, after a cancel, or on
/// failure.
///
std::optional<std::string> take_file();

} // namespace asw::dialog

#endif // ASW_DIALOG_H
