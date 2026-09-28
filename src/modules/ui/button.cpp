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
    // Track hover even while disabled, so the state is right when re-enabled
    if (e.type == UIEvent::Type::PointerEnter) {
        _hovered = true;
        return false;
    }
    if (e.type == UIEvent::Type::PointerLeave) {
        _hovered = false;
        _pressed = false;
        return false;
    }

    if (!enabled) {
        _pressed = false;
        return false;
    }

    switch (e.type) {
    case UIEvent::Type::PointerMove: {
        return false;
    }
    case UIEvent::Type::PointerDown: {
        if (e.mouse_button != asw::input::MouseButton::Left) {
            return false;
        }
        if (transform.contains(e.pointer_pos)) {
            _pressed = true;
            ctx.pointer_capture = this;
            ctx.focus.set_focus(ctx, this);
            return true;
        }
        return false;
    }
    case UIEvent::Type::PointerUp: {
        if (e.mouse_button != asw::input::MouseButton::Left) {
            return false;
        }
        const bool in = transform.contains(e.pointer_pos);
        const bool wasPressed = _pressed;
        _pressed = false;
        if (ctx.pointer_capture == this) {
            ctx.pointer_capture = nullptr;
        }
        if (wasPressed && in) {
            activate();
            return true;
        }
        return false;
    }
    case UIEvent::Type::Activate: {
        activate();
        return true;
    }
    default:
        break;
    }
    return false;
}

void asw::ui::Button::activate()
{
    if (on_click) {
        on_click();
    }
}

bool asw::ui::Button::is_highlighted(const Context& ctx) const
{
    // Root always focuses a widget, so focus alone only counts while it is shown
    return _pressed || _hovered || (_focused && ctx.theme.show_focus);
}

const asw::Texture& asw::ui::Button::current_texture(const Context& ctx) const
{
    if (!enabled) {
        return texture_disabled != nullptr ? texture_disabled : texture;
    }
    if (_pressed && texture_pressed != nullptr) {
        return texture_pressed;
    }
    if (is_highlighted(ctx) && texture_hover != nullptr) {
        return texture_hover;
    }
    return texture;
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
    const auto& s = get_style(ctx);
    const bool highlighted = enabled && is_highlighted(ctx);

    if (draw_background) {
        asw::Color bg = s.bg;
        if (!enabled) {
            bg = s.bg_disabled;
        } else if (_pressed) {
            bg = s.bg_pressed;
        } else if (highlighted) {
            bg = s.bg_hover;
        }
        asw::draw::rect_fill(transform, bg);
    }

    if (s.border_width > 0.0F) {
        asw::draw::rect(transform, s.border, s.border_width);
    }

    const asw::Quad<float> inner {
        { transform.position.x + padding, transform.position.y + padding },
        { transform.size.x - padding * 2.0f, transform.size.y - padding * 2.0f }
    };

    if (const auto& tex = current_texture(ctx); tex != nullptr) {
        asw::draw::stretch_sprite(tex, inner);
    }

    if (!text.empty() && font != nullptr) {
        asw::Color color = s.text;
        if (!enabled) {
            color = s.text_disabled;
        } else if (highlighted) {
            color = s.text_hover;
        }

        const auto text_size = asw::util::get_text_size(font, text);
        const float text_y = inner.position.y + ((inner.size.y - text_size.y) / 2.0F);

        float text_x = inner.get_center().x - (text_size.x / 2.0F);
        if (s.text_align == asw::TextJustify::Left) {
            text_x = inner.position.x;
        } else if (s.text_align == asw::TextJustify::Right) {
            text_x = inner.position.x + inner.size.x - text_size.x;
        }

        asw::draw::text(font, text, { text_x, text_y }, color, asw::TextJustify::Left);
    }

    draw_focus_ring(ctx.theme, transform, _focused);

    Widget::draw(ctx);
}
