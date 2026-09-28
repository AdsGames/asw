#include "./asw/modules/ui/checkbox.h"

#include <algorithm>

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::Checkbox::activate()
{
    checked = !checked;
    if (on_change) {
        on_change(checked);
    }
    Button::activate();
}

void asw::ui::Checkbox::draw(Context& ctx)
{
    const auto& s = get_checkbox_style(ctx);
    const bool highlighted = enabled && is_highlighted(ctx);

    // Row background and border, behind images too
    if (draw_background) {
        asw::draw::rect_fill(transform, highlighted ? s.bg_hover : s.bg);
    }
    if (s.border_width > 0.0F) {
        asw::draw::rect(transform, s.border, s.border_width);
    }

    const asw::Quad<float> inner {
        { transform.position.x + padding, transform.position.y + padding },
        { transform.size.x - (padding * 2.0F), transform.size.y - (padding * 2.0F) }
    };

    // Image checkbox
    const auto& unchecked_tex = current_texture(ctx);
    if (unchecked_tex != nullptr) {
        const asw::Texture* tex = &unchecked_tex;
        if (checked && enabled) {
            if (highlighted && texture_checked_hover != nullptr) {
                tex = &texture_checked_hover;
            } else if (texture_checked != nullptr) {
                tex = &texture_checked;
            }
        } else if (checked && texture_checked != nullptr) {
            tex = &texture_checked;
        }

        asw::draw::stretch_sprite(*tex, inner);
    } else {
        // Box as tall as the widget, text on the other side
        const float box_size = std::min(inner.size.x, inner.size.y);
        const bool box_right = s.box_side == BoxSide::Right;
        const float box_x = box_right ? inner.position.x + inner.size.x - box_size : inner.position.x;
        const asw::Quad<float> box { { box_x, inner.position.y }, { box_size, box_size } };

        asw::Color box_color = s.box;
        if (!enabled) {
            box_color = s.box_disabled;
        } else if (_pressed) {
            box_color = s.box_pressed;
        } else if (highlighted) {
            box_color = s.box_hover;
        }
        asw::draw::rect_fill(box, box_color);

        if (s.box_border_width > 0.0F) {
            asw::draw::rect(box, s.box_border, s.box_border_width);
        }

        if (checked) {
            const float inset = std::max(3.0F, box_size / 4.0F);
            const asw::Quad<float> mark { { box.position.x + inset, box.position.y + inset },
                { box_size - (inset * 2.0F), box_size - (inset * 2.0F) } };
            asw::draw::rect_fill(mark, enabled ? s.mark : s.mark_disabled);
        }

        if (!text.empty() && font != nullptr) {
            const auto text_size = asw::util::get_text_size(font, text);
            const float text_y
                = inner.position.y + ((inner.size.y - static_cast<float>(text_size.y)) / 2.0F);
            const float text_x
                = box_right ? inner.position.x : inner.position.x + box_size + ctx.theme.gap;
            asw::draw::text(font, text, { text_x, text_y }, enabled ? s.text : s.text_disabled,
                asw::TextJustify::Left);
        }
    }

    draw_focus_ring(ctx.theme, transform, _focused);

    // Children, skipping Button::draw which would draw the button again
    Widget::draw(ctx);
}
