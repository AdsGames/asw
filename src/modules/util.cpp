#include "./asw/modules/util.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "./asw/modules/log.h"

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
    TTF_Text* ttf_text = TTF_CreateText(nullptr, font.get(), text.c_str(), 0);
    asw::Vec2<int> size;
    TTF_GetTextSize(ttf_text, &size.x, &size.y);
    TTF_DestroyText(ttf_text);

    return size;
}