#include "./asw/modules/ui/theme.h"

#include "./asw/modules/draw.h"

void asw::ui::draw_focus_ring(const Theme& theme, const asw::Quad<float>& bounds, bool focused)
{
    const auto& ring = theme.focus_ring;
    if (!focused || !theme.show_focus || ring.width <= 0.0F || ring.color.a == 0) {
        return;
    }

    // Outline drawn inside the quad, so grow it by the gap and the width
    const float grow = ring.offset + ring.width;
    const asw::Quad<float> outer { { bounds.position.x - grow, bounds.position.y - grow },
        { bounds.size.x + (grow * 2.0F), bounds.size.y + (grow * 2.0F) } };
    asw::draw::rect(outer, ring.color, ring.width);
}
