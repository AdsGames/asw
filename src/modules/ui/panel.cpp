#include "./asw/modules/ui/panel.h"

#include "./asw/modules/draw.h"

void asw::ui::Panel::draw(Context& ctx)
{
    if (bg_image) {
        asw::draw::stretch_sprite(bg_image, transform);
    } else if (bg.a > 0) {
        // Skip see through fills, which would overwrite the scene when a game
        // leaves the draw blend mode off
        asw::draw::rect_fill(transform, bg);
    }

    Widget::draw(ctx);
}
