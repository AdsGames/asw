#include "./asw/modules/ui/input_box.h"

#include <SDL3/SDL.h>
#include <algorithm>

#include "./asw/modules/display.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::InputBox::on_focus_changed(Context& ctx, bool focused)
{
    focused_ = focused;
    (void)ctx;

    if (focused) {
        SDL_StartTextInput(asw::display::window);
        cursor_pos_ = value.size();
    } else {
        SDL_StopTextInput(asw::display::window);
    }
}

bool asw::ui::InputBox::on_event(Context& ctx, const UIEvent& e)
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
        return false;
    }
    case UIEvent::Type::PointerDown: {
        if (transform.contains(e.pointer_pos)) {
            ctx.pointer_capture = this;
            ctx.focus.set_focus(ctx, this);
            cursor_pos_ = value.size();
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
        value.insert(cursor_pos_, e.text);
        cursor_pos_ += e.text.size();
        if (on_change) {
            on_change(value);
        }
        return true;
    }
    case UIEvent::Type::KeyDown: {
        if (e.key == asw::input::Key::Backspace) {
            if (cursor_pos_ > 0) {
                value.erase(cursor_pos_ - 1, 1);
                cursor_pos_--;
                if (on_change) {
                    on_change(value);
                }
            }
            return true;
        }
        if (e.key == asw::input::Key::Delete) {
            if (cursor_pos_ < value.size()) {
                value.erase(cursor_pos_, 1);
                if (on_change) {
                    on_change(value);
                }
            }
            return true;
        }
        if (e.key == asw::input::Key::Left) {
            if (cursor_pos_ > 0) {
                cursor_pos_--;
            }
            return true;
        }
        if (e.key == asw::input::Key::Right) {
            if (cursor_pos_ < value.size()) {
                cursor_pos_++;
            }
            return true;
        }
        if (e.key == asw::input::Key::Home) {
            cursor_pos_ = 0;
            return true;
        }
        if (e.key == asw::input::Key::End) {
            cursor_pos_ = value.size();
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

    // Background
    asw::Color bg = ctx.theme.input_bg;
    if (!enabled) {
        bg = ctx.theme.panel_bg;
    }
    asw::draw::rect_fill(transform, bg);

    // Border
    asw::Color border = ctx.theme.btn_bg;
    if (hovered_ && enabled) {
        border = ctx.theme.btn_hover;
    }
    asw::draw::rect(transform, border);

    // Clip text to input bounds
    const SDL_Rect clip {
        static_cast<int>(transform.position.x + text_padding),
        static_cast<int>(transform.position.y),
        static_cast<int>(transform.size.x - (text_padding * 2)),
        static_cast<int>(transform.size.y),
    };
    SDL_SetRenderClipRect(asw::display::renderer, &clip);

    // Text position (vertically centered)
    const auto display_text = value.empty() ? placeholder : value;
    const auto display_color = value.empty() ? ctx.theme.text_dim : ctx.theme.text;

    if (!display_text.empty() && font != nullptr) {
        const auto text_size = asw::util::get_text_size(font, display_text);
        const float text_y = transform.position.y + ((transform.size.y - text_size.y) / 2.0F);
        const asw::Vec2 text_pos { transform.position.x + text_padding, text_y };

        asw::draw::text(font, display_text, text_pos, display_color);
    }

    // Cursor
    if (focused_ && font != nullptr) {
        const auto before_cursor = value.substr(0, cursor_pos_);
        float cursor_x = transform.position.x + text_padding;

        if (!before_cursor.empty()) {
            const auto size = asw::util::get_text_size(font, before_cursor);
            cursor_x += static_cast<float>(size.x);
        }

        const auto text_height = asw::util::get_text_size(font, "|");
        const float cursor_y = transform.position.y
            + ((transform.size.y - static_cast<float>(text_height.y)) / 2.0F);

        asw::draw::line({ cursor_x, cursor_y },
            { cursor_x, cursor_y + static_cast<float>(text_height.y) }, ctx.theme.text);
    }

    // Reset clip
    SDL_SetRenderClipRect(asw::display::renderer, nullptr);

    // Focus ring
    if (focused_ && ctx.theme.show_focus) {
        auto ring = asw::Quad<float>(transform);
        ring.position.x -= 2;
        ring.position.y -= 2;
        ring.size.x += 4;
        ring.size.y += 4;
        asw::draw::rect(ring, ctx.theme.btn_focus_ring);
    }

    Widget::draw(ctx);
}
