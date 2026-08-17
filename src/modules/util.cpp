#include "./asw/modules/util.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>

#include "./asw/modules/log.h"

namespace {
struct TextSizeCacheKey {
    asw::Font font;
    std::string text;

    bool operator==(const TextSizeCacheKey&) const = default;
};

struct TextSizeCacheKeyHash {
    std::size_t operator()(const TextSizeCacheKey& key) const
    {
        std::size_t seed = std::hash<asw::Font> {}(key.font);
        seed ^= std::hash<std::string> {}(key.text) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        return seed;
    }
};

constexpr std::size_t TEXT_SIZE_CACHE_LIMIT = 512;
std::unordered_map<TextSizeCacheKey, asw::Vec2<int>, TextSizeCacheKeyHash> text_size_cache;
} // namespace

void asw::util::abort_on_error(const std::string& message)
{
    asw::log::error(message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), nullptr);
    exit(-1);
}

asw::Vec2<float> asw::util::get_texture_size(const asw::Texture& tex)
{
    asw::Vec2<float> size;
    SDL_GetTextureSize(tex.get(), &size.x, &size.y);
    return size;
}

asw::Vec2<int> asw::util::get_text_size(const asw::Font& font, const std::string& text)
{
    if (font == nullptr) {
        return {};
    }

    const TextSizeCacheKey cache_key { font, text };
    if (auto it = text_size_cache.find(cache_key); it != text_size_cache.end()) {
        return it->second;
    }

    TTF_Text* ttf_text = TTF_CreateText(nullptr, font.get(), text.c_str(), 0);
    asw::Vec2<int> size;
    TTF_GetTextSize(ttf_text, &size.x, &size.y);
    TTF_DestroyText(ttf_text);

    if (text_size_cache.size() >= TEXT_SIZE_CACHE_LIMIT) {
        // Keep the shared cache bounded while still avoiding repeated
        // measurement work for the common stable UI strings.
        text_size_cache.clear();
    }

    text_size_cache.try_emplace(cache_key, size);
    return size;
}

void asw::util::clear_text_size_cache()
{
    text_size_cache.clear();
}