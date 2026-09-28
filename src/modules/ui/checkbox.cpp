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
    const bool active = enabled && (_pressed || _hovered || _focused);

    // Image checkbox
    const auto& unchecked_tex = current_texture();
    if (unchecked_tex != nullptr) {
        const asw::Texture* tex = &unchecked_tex;
        if (checked && enabled) {
            if (active && texture_checked_hover != nullptr) {
                tex = &texture_checked_hover;
            } else if (texture_checked != nullptr) {
                tex = &texture_checked;
            }
        } else if (checked && texture_checked != nullptr) {
            tex = &texture_checked;
        }

        if (draw_background) {
            asw::draw::rect_fill(transform, ctx.theme.btn_bg);
        }
        asw::draw::stretch_sprite(*tex, transform);
    } else {
        // Box as tall as the widget, text to its right
        const float box_size = std::min(transform.size.x, transform.size.y);
        const asw::Quad<float> box { transform.position, { box_size, box_size } };

        asw::Color bg = ctx.theme.btn_bg;
        if (!enabled) {
            bg = ctx.theme.panel_bg;
        } else if (_pressed) {
            bg = ctx.theme.btn_pressed;
        } else if (_hovered) {
            bg = ctx.theme.btn_hover;
        }
        asw::draw::rect_fill(box, bg);

        if (checked) {
            const float inset = std::max(3.0F, box_size / 4.0F);
            const asw::Quad<float> mark { { box.position.x + inset, box.position.y + inset },
                { box_size - inset * 2.0F, box_size - inset * 2.0F } };
            asw::draw::rect_fill(mark, enabled ? ctx.theme.text : ctx.theme.text_dim);
        }

        if (!text.empty() && font != nullptr) {
            const auto text_size = asw::util::get_text_size(font, text);
            const asw::Vec2<float> text_pos { box.position.x + box_size + ctx.theme.gap,
                transform.position.y + (transform.size.y - text_size.y) / 2.0F };
            asw::draw::text(font, text, text_pos, enabled ? ctx.theme.text : ctx.theme.text_dim,
                asw::TextJustify::Left);
        }
    }

    if (_focused && ctx.theme.show_focus) {
        auto ring = asw::Quad<float>(transform);
        ring.position.x -= 2;
        ring.position.y -= 2;
        ring.size.x += 4;
        ring.size.y += 4;
        asw::draw::rect(ring, ctx.theme.btn_focus_ring);
    }

    // Children, skipping Button::draw which would draw the button again
    Widget::draw(ctx);
}
