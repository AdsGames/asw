#include "./asw/modules/util.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string_view>

#include "./asw/modules/log.h"
#include "./lru_cache.h"

namespace {
struct TextSizeCacheKey {
    // Holds the font, so its address is not reused while cached
    asw::Font font;
    std::string text;

    // Changes when the font's size, style or hinting change
    uint32_t font_generation;
};

// What a lookup compares, so a lookup does not copy the text
struct TextSizeKeyView {
    const TTF_Font* font;
    std::string_view text;
    uint32_t font_generation;

    bool operator==(const TextSizeKeyView&) const = default;
};

TextSizeKeyView view(const TextSizeCacheKey& key)
{
    return { key.font.get(), key.text, key.font_generation };
}

const TextSizeKeyView& view(const TextSizeKeyView& key)
{
    return key;
}

struct TextSizeKeyHash {
    using is_transparent = void;

    template <typename K> std::size_t operator()(const K& key) const
    {
        const TextSizeKeyView v = view(key);
        std::size_t seed = std::hash<const TTF_Font*> { }(v.font);
        asw::detail::hash_combine(seed, std::hash<std::string_view> { }(v.text));
        asw::detail::hash_combine(seed, std::hash<uint32_t> { }(v.font_generation));
        return seed;
    }
};

struct TextSizeKeyEqual {
    using is_transparent = void;

    template <typename A, typename B> bool operator()(const A& a, const B& b) const
    {
        return view(a) == view(b);
    }
};

constexpr std::size_t TEXT_SIZE_CACHE_LIMIT = 512;
asw::detail::LruCache<TextSizeCacheKey, asw::Vec2<int>, TextSizeKeyHash, TextSizeKeyEqual>
    text_size_cache(TEXT_SIZE_CACHE_LIMIT);
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

asw::Vec2<int> asw::util::get_text_size(const asw::Font& font, std::string_view text)
{
    if (font == nullptr) {
        return {};
    }

    const uint32_t font_generation = TTF_GetFontGeneration(font.get());
    if (const auto* cached
        = text_size_cache.find(TextSizeKeyView { font.get(), text, font_generation })) {
        return *cached;
    }

    // Length given, so the text does not need to end in a null. A length of
    // 0 means null terminated to SDL_ttf, so empty text is passed as "".
    TTF_Text* ttf_text = text.empty()
        ? TTF_CreateText(nullptr, font.get(), "", 0)
        : TTF_CreateText(nullptr, font.get(), text.data(), text.size());
    asw::Vec2<int> size;
    TTF_GetTextSize(ttf_text, &size.x, &size.y);
    TTF_DestroyText(ttf_text);

    text_size_cache.insert({ font, std::string(text), font_generation }, size);
    return size;
}

int asw::util::get_font_height(const asw::Font& font)
{
    if (font == nullptr) {
        return 0;
    }

    return TTF_GetFontHeight(font.get());
}

void asw::util::clear_text_size_cache()
{
    text_size_cache.clear();
}

bool asw::util::open_url(const std::string& url)
{
    if (!SDL_OpenURL(url.c_str())) {
        asw::log::warn("Could not open " + url + ": " + SDL_GetError());
        return false;
    }
    return true;
}
