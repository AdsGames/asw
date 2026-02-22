#include "./asw/modules/ui/vbox.h"

void asw::ui::VBox::layout(Context& ctx) {
  float y = transform.position.y + padding;
  const float x = transform.position.x + padding;
  const float w = transform.size.x - (padding * 2.0F);

  for (auto const& c : children) {
    if (!c->visible) {
      continue;
    }

    c->transform.position.x = x;
    c->transform.position.y = y;
    c->transform.size.x = w;
    c->layout(ctx);
    y += c->transform.size.y + gap;
  }
}
