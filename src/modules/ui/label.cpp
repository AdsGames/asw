#include "./asw/modules/ui/label.h"

#include "./asw/modules/draw.h"

void asw::ui::Label::draw(Context& ctx)
{
    if (!text.empty() && font != nullptr) {
        asw::draw::text(font, text, transform.position, color, justify);
    }

    Widget::draw(ctx);
}
