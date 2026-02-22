#include "./asw/modules/ui/panel.h"

#include "./asw/modules/draw.h"

void asw::ui::Panel::draw(Context& ctx) {
  if (bg_image) {
    asw::draw::stretch_sprite(bg_image, transform);
  } else {
    asw::draw::rect_fill(transform, bg);
  }

  Widget::draw(ctx);
}
