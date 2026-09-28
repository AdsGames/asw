#include "./asw/modules/dialog.h"

#include <SDL3/SDL.h>
#include <array>
#include <atomic>
#include <mutex>
#include <utility>

#include "./asw/modules/display.h"
#include "./asw/modules/log.h"

namespace {
// SDL may run the file chooser callback on another thread
std::mutex file_mutex;
std::optional<std::string> chosen_file;
std::atomic<bool> file_pending = false;

// SDL reads the filters until the chooser closes, so they live here
std::vector<asw::dialog::FileFilter> active_filters;
std::vector<SDL_DialogFileFilter> active_sdl_filters;

void SDLCALL on_file_chosen(void* /*userdata*/, const char* const* files, int /*filter*/)
{
    if (files == nullptr) {
        asw::log::warn("File chooser failed: {}", SDL_GetError());
    } else if (files[0] != nullptr) {
        const std::scoped_lock lock(file_mutex);
        chosen_file = files[0];
    }

    file_pending = false;
}

void show_message(SDL_MessageBoxFlags flags, const std::string& title, const std::string& message)
{
    SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), asw::display::get_window());
}
} // namespace

void asw::dialog::info(const std::string& title, const std::string& message)
{
    show_message(SDL_MESSAGEBOX_INFORMATION, title, message);
}

void asw::dialog::warn(const std::string& title, const std::string& message)
{
    show_message(SDL_MESSAGEBOX_WARNING, title, message);
}

void asw::dialog::error(const std::string& title, const std::string& message)
{
    show_message(SDL_MESSAGEBOX_ERROR, title, message);
}

bool asw::dialog::confirm(const std::string& title, const std::string& message)
{
    const std::array buttons {
        SDL_MessageBoxButtonData { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
        SDL_MessageBoxButtonData { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    };

    const SDL_MessageBoxData data {
        SDL_MESSAGEBOX_WARNING,
        asw::display::get_window(),
        title.c_str(),
        message.c_str(),
        static_cast<int>(buttons.size()),
        buttons.data(),
        nullptr,
    };

    int pressed = 0;
    return SDL_ShowMessageBox(&data, &pressed) && pressed == 1;
}

bool asw::dialog::request_file(
    FileMode mode, const std::string& default_location, const std::vector<FileFilter>& filters)
{
    if (file_pending) {
        return false;
    }

    {
        const std::scoped_lock lock(file_mutex);
        chosen_file.reset();
    }

    active_filters = filters;
    active_sdl_filters.clear();
    for (const auto& filter : active_filters) {
        active_sdl_filters.push_back({ filter.name.c_str(), filter.pattern.c_str() });
    }

    const auto* sdl_filters = active_sdl_filters.empty() ? nullptr : active_sdl_filters.data();
    const auto filter_count = static_cast<int>(active_sdl_filters.size());
    const char* location = default_location.empty() ? nullptr : default_location.c_str();

    // Set before showing, the callback can run before the call returns
    file_pending = true;

    if (mode == FileMode::Save) {
        SDL_ShowSaveFileDialog(on_file_chosen, nullptr, asw::display::get_window(), sdl_filters,
            filter_count, location);
    } else {
        SDL_ShowOpenFileDialog(on_file_chosen, nullptr, asw::display::get_window(), sdl_filters,
            filter_count, location, false);
    }

    return true;
}

bool asw::dialog::is_file_pending()
{
    return file_pending;
}

std::optional<std::string> asw::dialog::take_file()
{
    const std::scoped_lock lock(file_mutex);
    return std::exchange(chosen_file, std::nullopt);
}
