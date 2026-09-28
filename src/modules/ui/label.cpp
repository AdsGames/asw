#include "./asw/modules/ui/label.h"

#include "./asw/modules/draw.h"

void asw::ui::Label::draw(Context& ctx)
{
    if (const auto& f = pick_font(font, ctx.theme); !text.empty() && f != nullptr) {
        asw::draw::text(f, text, transform.position, color.value_or(ctx.theme.text), justify);
    }

    Widget::draw(ctx);
}
