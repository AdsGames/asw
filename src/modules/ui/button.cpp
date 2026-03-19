#include "./asw/modules/ui/button.h"

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::Button::on_focus_changed(Context& ctx, bool focused)
{
    _focused = focused;
    (void)ctx;
}

bool asw::ui::Button::on_event(Context& ctx, const UIEvent& e)
{
    if (!enabled) {
        return false;
    }

    switch (e.type) {
    case UIEvent::Type::PointerEnter: {
        _hovered = true;
        return false;
    }
    case UIEvent::Type::PointerLeave: {
        _hovered = false;
        _pressed = false;
        return false;
    }
    case UIEvent::Type::PointerMove: {
        return false;
    }
    case UIEvent::Type::PointerDown: {
        if (transform.contains(e.pointer_pos)) {
            _pressed = true;
            ctx.pointer_capture = this;
            ctx.focus.set_focus(ctx, this);
            return true;
        }
        return false;
    }
    case UIEvent::Type::PointerUp: {
        const bool in = transform.contains(e.pointer_pos);
        const bool wasPressed = _pressed;
        _pressed = false;
        if (ctx.pointer_capture == this) {
            ctx.pointer_capture = nullptr;
        }
        if (wasPressed && in) {
            if (on_click) {
                on_click();
            }
            return true;
        }
        return false;
    }
    case UIEvent::Type::Activate: {
        if (on_click) {
            on_click();
        }
        return true;
    }
    default:
        break;
    }
    return false;
}

void asw::ui::Button::set_texture(const asw::Texture& tex, bool auto_size)
{
    texture = tex;
    if (auto_size && texture != nullptr) {
        const auto tex_size = asw::util::get_texture_size(texture);
        transform.size = tex_size + asw::Vec2<float>(padding * 2.0f, padding * 2.0f);
    }
}

void asw::ui::Button::set_text(const std::string& t, bool auto_size)
{
    text = t;
    if (auto_size && font != nullptr && !text.empty()) {
        const auto size = asw::util::get_text_size(font, text);
        transform.size = asw::Vec2<float>(
            size.x + padding * 2.0f,
            size.y + padding * 2.0f);
    }
}

void asw::ui::Button::draw(Context& ctx)
{
    asw::Color bg = ctx.theme.btn_bg;
    if (!enabled) {
        bg = ctx.theme.panel_bg;
    } else if (_pressed) {
        bg = ctx.theme.btn_pressed;
    } else if (_hovered) {
        bg = ctx.theme.btn_hover;
    }

    asw::draw::rect_fill(transform, bg);

    const asw::Quad<float> inner {
        { transform.position.x + padding, transform.position.y + padding },
        { transform.size.x - padding * 2.0f, transform.size.y - padding * 2.0f }
    };

    if (texture != nullptr) {
        asw::draw::stretch_sprite(texture, inner);
    }

    if (!text.empty() && font != nullptr) {
        const auto text_size = asw::util::get_text_size(font, text);
        const auto text_pos
            = inner.get_center() - asw::Vec2<float>(text_size.x / 2.0f, text_size.y / 2.0f);

        asw::draw::text(font, text, text_pos, ctx.theme.text, asw::TextJustify::Left);
    }

    if (_focused && ctx.theme.show_focus) {
        // Focus ring
        auto ring = asw::Quad<float>(transform);
        ring.position.x -= 2;
        ring.position.y -= 2;
        ring.size.x += 4;
        ring.size.y += 4;
        asw::draw::rect(ring, ctx.theme.btn_focus_ring);
    }

    Widget::draw(ctx);
}
