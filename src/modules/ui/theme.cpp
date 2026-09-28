#include "./asw/modules/ui/theme.h"

#include "./asw/modules/draw.h"

void asw::ui::draw_focus_ring(const FocusRingStyle& style, const asw::Quad<float>& bounds)
{
    if (style.width <= 0.0F || style.color.a == 0) {
        return;
    }

    // Outline drawn inside the quad, so grow it by the gap and the width
    const float grow = style.offset + style.width;
    const asw::Quad<float> outer { { bounds.position.x - grow, bounds.position.y - grow },
        { bounds.size.x + (grow * 2.0F), bounds.size.y + (grow * 2.0F) } };
    asw::draw::rect(outer, style.color, style.width);
}
