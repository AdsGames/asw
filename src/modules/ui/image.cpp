#include "./asw/modules/ui/image.h"

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::Image::set_texture(const asw::Texture& tex, bool auto_size)
{
    texture = tex;
    if (auto_size && texture != nullptr) {
        transform.size = asw::util::get_texture_size(texture);
    }
}

void asw::ui::Image::draw(Context& ctx)
{
    if (texture != nullptr) {
        asw::draw::stretch_sprite(texture, transform);
    }

    Widget::draw(ctx);
}
