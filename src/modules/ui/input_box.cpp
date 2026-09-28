#include "./asw/modules/ui/input_box.h"

#include <SDL3/SDL.h>
#include <algorithm>

#include "./asw/modules/display.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

namespace {
// UTF-8 continuation bytes look like 10xxxxxx
bool is_continuation(char c)
{
    return (static_cast<unsigned char>(c) & 0xC0) == 0x80;
}

// Start of the character before pos
std::size_t prev_char(const std::string& s, std::size_t pos)
{
    if (pos == 0) {
        return 0;
    }
    --pos;
    while (pos > 0 && is_continuation(s[pos])) {
        --pos;
    }
    return pos;
}

// Start of the character after the one at pos
std::size_t next_char(const std::string& s, std::size_t pos)
{
    if (pos >= s.size()) {
        return s.size();
    }
    ++pos;
    while (pos < s.size() && is_continuation(s[pos])) {
        ++pos;
    }
    return pos;
}
} // namespace

asw::ui::InputBox::~InputBox()
{
    if (_focused) {
        SDL_StopTextInput(asw::display::get_window());
    }
}

void asw::ui::InputBox::on_focus_changed(Context& ctx, bool focused)
{
    _focused = focused;
    (void)ctx;

    if (focused) {
        SDL_StartTextInput(asw::display::get_window());
        _cursor_pos = value.size();
    } else {
        SDL_StopTextInput(asw::display::get_window());
    }
}

bool asw::ui::InputBox::on_event(Context& ctx, const UIEvent& e)
{
    // value is public and may have been shortened since the last event
    _cursor_pos = std::min(_cursor_pos, value.size());

    // Track hover even while disabled, so the state is right when re-enabled
    if (e.type == UIEvent::Type::PointerEnter) {
        _hovered = true;
        return false;
    }
    if (e.type == UIEvent::Type::PointerLeave) {
        _hovered = false;
        return false;
    }

    if (!enabled) {
        return false;
    }

    switch (e.type) {
    case UIEvent::Type::PointerDown: {
        if (e.mouse_button != asw::input::MouseButton::Left) {
            return false;
        }
        if (transform.contains(e.pointer_pos)) {
            ctx.pointer_capture = this;
            ctx.focus.set_focus(ctx, this);
            _cursor_pos = value.size();
            return true;
        }
        return false;
    }
    case UIEvent::Type::PointerUp: {
        if (ctx.pointer_capture == this) {
            ctx.pointer_capture = nullptr;
        }
        return false;
    }
    case UIEvent::Type::TextInput: {
        value.insert(_cursor_pos, e.text);
        _cursor_pos += e.text.size();
        if (on_change) {
            on_change(value);
        }
        return true;
    }
    case UIEvent::Type::KeyDown: {
        if (e.key == asw::input::Key::Backspace) {
            if (_cursor_pos > 0) {
                // Whole characters, not single bytes of a UTF-8 sequence
                const auto start = prev_char(value, _cursor_pos);
                value.erase(start, _cursor_pos - start);
                _cursor_pos = start;
                if (on_change) {
                    on_change(value);
                }
            }
            return true;
        }
        if (e.key == asw::input::Key::Delete) {
            if (_cursor_pos < value.size()) {
                value.erase(_cursor_pos, next_char(value, _cursor_pos) - _cursor_pos);
                if (on_change) {
                    on_change(value);
                }
            }
            return true;
        }
        if (e.key == asw::input::Key::Left) {
            _cursor_pos = prev_char(value, _cursor_pos);
            return true;
        }
        if (e.key == asw::input::Key::Right) {
            _cursor_pos = next_char(value, _cursor_pos);
            return true;
        }
        if (e.key == asw::input::Key::Home) {
            _cursor_pos = 0;
            return true;
        }
        if (e.key == asw::input::Key::End) {
            _cursor_pos = value.size();
            return true;
        }
        return false;
    }
    case UIEvent::Type::Activate: {
        // Consume activate to prevent Space from triggering other actions
        return true;
    }
    default:
        break;
    }
    return false;
}

void asw::ui::InputBox::draw(Context& ctx)
{
    constexpr float text_padding = 4.0F;

    const auto& s = get_style(ctx);

    // Background
    asw::draw::rect_fill(transform, enabled ? s.bg : s.bg_disabled);

    // Border
    if (s.border_width > 0.0F) {
        asw::draw::rect(
            transform, (_hovered && enabled) ? s.border_hover : s.border, s.border_width);
    }

    // Clip text to input bounds
    const SDL_Rect clip {
        static_cast<int>(transform.position.x + text_padding),
        static_cast<int>(transform.position.y),
        static_cast<int>(transform.size.x - (text_padding * 2)),
        static_cast<int>(transform.size.y),
    };
    SDL_SetRenderClipRect(asw::display::get_renderer(), &clip);

    // Text position (vertically centered)
    const auto display_text = value.empty() ? placeholder : value;
    const auto display_color = value.empty() ? s.placeholder : s.text;

    if (!display_text.empty() && font != nullptr) {
        const auto text_size = asw::util::get_text_size(font, display_text);
        const float text_y = transform.position.y + ((transform.size.y - text_size.y) / 2.0F);
        const asw::Vec2 text_pos { transform.position.x + text_padding, text_y };

        asw::draw::text(font, display_text, text_pos, display_color);
    }

    // Cursor
    _cursor_pos = std::min(_cursor_pos, value.size());
    if (_focused && font != nullptr) {
        const auto before_cursor = value.substr(0, _cursor_pos);
        float cursor_x = transform.position.x + text_padding;

        if (!before_cursor.empty()) {
            const auto size = asw::util::get_text_size(font, before_cursor);
            cursor_x += static_cast<float>(size.x);
        }

        const auto text_height = asw::util::get_text_size(font, "|");
        const float cursor_y = transform.position.y
            + ((transform.size.y - static_cast<float>(text_height.y)) / 2.0F);

        asw::draw::line({ cursor_x, cursor_y },
            { cursor_x, cursor_y + static_cast<float>(text_height.y) }, s.caret);
    }

    // Reset clip
    SDL_SetRenderClipRect(asw::display::get_renderer(), nullptr);

    // Focus ring
    if (_focused && ctx.show_focus) {
        draw_focus_ring(ctx.theme.focus_ring, transform);
    }

    Widget::draw(ctx);
}
