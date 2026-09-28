#include "./asw/modules/ui/slider.h"

#include <algorithm>
#include <cmath>

#include "./asw/modules/draw.h"
#include "./asw/modules/ui/layout.h"

void asw::ui::Slider::set_value(float v)
{
    const float lo = std::min(min, max);
    const float hi = std::max(min, max);
    value = std::clamp(v, lo, hi);
}

bool asw::ui::Slider::adjusts_left_right() const
{
    return adjust_on_left_right.value_or(!in_row_with_focusables(*this));
}

void asw::ui::Slider::activate(Context& ctx)
{
    // Only keyboard and controller activation edits, clicks set the value
    if (!ctx.show_focus) {
        return;
    }
    _editing = !adjusts_left_right() && !_editing;
}

void asw::ui::Slider::on_focus_changed(Context& ctx, bool focused)
{
    (void)ctx;
    if (!focused) {
        _editing = false;
    }
}

void asw::ui::Slider::change(float v)
{
    const float old = value;
    set_value(v);
    if (value != old && on_change) {
        on_change(value);
    }
}

void asw::ui::Slider::set_from_pointer(float x)
{
    const float range = max - min;
    const float width = transform.size.x;
    if (width <= 0.0F || range == 0.0F) {
        return;
    }

    float v = min + (std::clamp((x - transform.position.x) / width, 0.0F, 1.0F) * range);
    if (snap && step > 0.0F) {
        v = min + (std::round((v - min) / step) * step);
    }
    change(v);
}

bool asw::ui::Slider::on_event(Context& ctx, const UIEvent& e)
{
    (void)ctx;
    if (!enabled) {
        return false;
    }

    switch (e.type) {
    case UIEvent::Type::PointerDown:
        if (e.mouse_button != asw::input::MouseButton::Left) {
            return false;
        }
        set_from_pointer(e.pointer_pos.x);
        return true;
    case UIEvent::Type::PointerMove:
        // Root sends moves to the pressed widget while the button is held
        if (_captured) {
            set_from_pointer(e.pointer_pos.x);
            return true;
        }
        return false;
    case UIEvent::Type::Back:
        if (_editing) {
            _editing = false;
            return true;
        }
        return false;
    case UIEvent::Type::KeyDown:
        if (!adjusts_left_right() && !_editing) {
            return false;
        }
        if (e.key == asw::input::Key::Left) {
            change(value - step);
            return true;
        }
        if (e.key == asw::input::Key::Right) {
            change(value + step);
            return true;
        }
        return false;
    default:
        return false;
    }
}

void asw::ui::Slider::draw(Context& ctx)
{
    const auto& s = get_style(ctx);
    const float range = max - min;
    const float t = range == 0.0F ? 0.0F : std::clamp((value - min) / range, 0.0F, 1.0F);

    const float track_y = transform.position.y + ((transform.size.y - s.track_height) / 2.0F);
    const asw::Quad<float> track { { transform.position.x, track_y },
        { transform.size.x, s.track_height } };
    asw::draw::rect_fill(track, s.track);

    const asw::Quad<float> fill { track.position, { transform.size.x * t, s.track_height } };
    asw::draw::rect_fill(fill, enabled ? s.fill : s.disabled);

    const float knob_x = transform.position.x + (transform.size.x * t) - (s.knob_width / 2.0F);
    const asw::Quad<float> knob { { knob_x, transform.position.y },
        { s.knob_width, transform.size.y } };

    asw::Color knob_color = s.knob;
    if (!enabled) {
        knob_color = s.disabled;
    } else if (_editing) {
        knob_color = s.knob_editing;
    } else if (is_highlighted(ctx)) {
        knob_color = s.knob_hover;
    }
    asw::draw::rect_fill(knob, knob_color);

    Widget::draw(ctx);
}
