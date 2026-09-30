#include "./asw/modules/config.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <map>
#include <string_view>

#include "./asw/modules/log.h"

namespace {
std::map<std::string, std::string, std::less<>> settings;

std::string_view trim(std::string_view text)
{
    const auto is_space = [](char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; };
    while (!text.empty() && is_space(text.front())) {
        text.remove_prefix(1);
    }
    while (!text.empty() && is_space(text.back())) {
        text.remove_suffix(1);
    }
    return text;
}

std::string lower(std::string_view text)
{
    std::string result(text);
    std::ranges::transform(result, result.begin(),
        [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
    return result;
}

void parse(std::string_view text, const std::string& path)
{
    int line_number = 0;
    while (!text.empty()) {
        const auto end = text.find('\n');
        const auto raw = text.substr(0, end);
        text = end == std::string_view::npos ? std::string_view { } : text.substr(end + 1);
        ++line_number;

        const auto line = trim(raw);
        if (line.empty() || line.front() == '#' || line.front() == ';') {
            continue;
        }

        const auto equals = line.find('=');
        const auto key = trim(line.substr(0, equals));
        if (equals == std::string_view::npos || key.empty()) {
            asw::log::warn("{}:{}: ignoring line, use key = value", path, line_number);
            continue;
        }

        const auto value = trim(line.substr(equals + 1));
        settings.insert_or_assign(std::string(key), std::string(value));
        asw::log::info("Config: {} = {}", key, value);
    }
}
} // namespace

void asw::config::_init()
{
    settings.clear();

    const char* path = SDL_getenv(PATH_VARIABLE);
    if (path != nullptr && *path != '\0') {
        load(path);
    }
}

void asw::config::_shutdown()
{
    settings.clear();
}

bool asw::config::load(const std::string& path)
{
    std::size_t size = 0;
    void* data = SDL_LoadFile(path.c_str(), &size);
    if (data == nullptr) {
        asw::log::warn("Could not read config {}: {}", path, SDL_GetError());
        return false;
    }

    parse(std::string_view(static_cast<const char*>(data), size), path);
    SDL_free(data);
    return true;
}

bool asw::config::has(const std::string& key)
{
    return settings.contains(key);
}

std::optional<std::string> asw::config::get_string(const std::string& key)
{
    const auto it = settings.find(key);
    if (it == settings.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<bool> asw::config::get_bool(const std::string& key)
{
    const auto value = get_string(key);
    if (!value) {
        return std::nullopt;
    }

    const auto v = lower(*value);
    if (v == "1" || v == "true" || v == "yes" || v == "on") {
        return true;
    }
    if (v == "0" || v == "false" || v == "no" || v == "off") {
        return false;
    }

    asw::log::warn("Ignoring config {} = {}, use true or false", key, *value);
    return std::nullopt;
}

std::optional<int> asw::config::get_int(const std::string& key)
{
    const auto value = get_string(key);
    if (!value) {
        return std::nullopt;
    }

    char* end = nullptr;
    errno = 0;
    const long parsed = std::strtol(value->c_str(), &end, 10);
    if (value->empty() || *end != '\0' || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        asw::log::warn("Ignoring config {} = {}, use a whole number", key, *value);
        return std::nullopt;
    }

    return static_cast<int>(parsed);
}

std::optional<float> asw::config::get_float(const std::string& key)
{
    const auto value = get_string(key);
    if (!value) {
        return std::nullopt;
    }

    char* end = nullptr;
    errno = 0;
    const float parsed = std::strtof(value->c_str(), &end);
    if (value->empty() || *end != '\0' || errno == ERANGE) {
        asw::log::warn("Ignoring config {} = {}, use a number", key, *value);
        return std::nullopt;
    }

    return parsed;
}
