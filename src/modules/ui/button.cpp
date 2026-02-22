#include "./asw/modules/ui/button.h"

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::Button::on_focus_changed(Context& ctx, bool focused)
{
    focused_ = focused;
    (void)ctx;
}

bool asw::ui::Button::on_event(Context& ctx, const UIEvent& e)
{
    if (!enabled) {
        return false;
    }

    switch (e.type) {
    case UIEvent::Type::PointerEnter: {
        hovered_ = true;
        return false;
    }
    case UIEvent::Type::PointerLeave: {
        hovered_ = false;
        pressed_ = false;
        return false;
    }
    case UIEvent::Type::PointerMove: {
        return false;
    }
    case UIEvent::Type::PointerDown: {
        if (transform.contains(e.pointer_pos)) {
            pressed_ = true;
            ctx.pointer_capture = this;
            ctx.focus.set_focus(ctx, this);
            return true;
        }
        return false;
    }
    case UIEvent::Type::PointerUp: {
        const bool in = transform.contains(e.pointer_pos);
        const bool wasPressed = pressed_;
        pressed_ = false;
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

void asw::ui::Button::draw(Context& ctx)
{
    asw::Color bg = ctx.theme.btn_bg;
    if (!enabled) {
        bg = ctx.theme.panel_bg;
    } else if (pressed_) {
        bg = ctx.theme.btn_pressed;
    } else if (hovered_) {
        bg = ctx.theme.btn_hover;
    }

    asw::draw::rect_fill(transform, bg);

    if (!text.empty() && font != nullptr) {
        const auto text_size = asw::util::get_text_size(font, text);
        const auto text_pos
            = transform.get_center() - asw::Vec2<float>(text_size.x / 2.0f, text_size.y / 2.0f);

        asw::draw::text(font, text, text_pos, ctx.theme.text, asw::TextJustify::Left);
    }

    if (focused_ && ctx.theme.show_focus) {
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
