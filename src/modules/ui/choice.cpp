#include "./asw/modules/ui/choice.h"

#include <algorithm>

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

std::size_t asw::ui::Choice::count() const
{
    return std::max(options.size(), images.size());
}

void asw::ui::Choice::select(std::size_t i)
{
    const auto n = count();
    index = n == 0 ? 0 : std::min(i, n - 1);
}

void asw::ui::Choice::step(int dir)
{
    const auto n = count();
    if (n == 0) {
        return;
    }

    std::size_t next_index = index;
    if (dir > 0) {
        if (index + 1 < n) {
            next_index = index + 1;
        } else if (wrap) {
            next_index = 0;
        }
    } else {
        if (index > 0) {
            next_index = index - 1;
        } else if (wrap) {
            next_index = n - 1;
        }
    }

    if (next_index != index) {
        index = next_index;
        if (on_change) {
            on_change(index);
        }
    }
}

void asw::ui::Choice::next()
{
    step(1);
}

void asw::ui::Choice::prev()
{
    step(-1);
}

void asw::ui::Choice::activate(Context& ctx)
{
    next();
    Button::activate(ctx);
}

bool asw::ui::Choice::on_event(Context& ctx, const UIEvent& e)
{
    (void)ctx;
    if (!enabled || e.type != UIEvent::Type::KeyDown) {
        return false;
    }
    if (e.key == asw::input::Key::Left) {
        prev();
        return true;
    }
    if (e.key == asw::input::Key::Right) {
        next();
        return true;
    }
    return false;
}

void asw::ui::Choice::draw(Context& ctx)
{
    select(index);

    // Show the selected option through the Button fields
    text = index < options.size() ? options[index] : std::string {};
    texture = index < images.size() ? images[index] : asw::Texture {};

    Button::draw(ctx);

    const auto& f = pick_font(font, ctx.theme);
    if (!show_arrows || text.empty() || f == nullptr || !enabled || !is_highlighted(ctx)) {
        return;
    }

    const auto& s = get_style(ctx);
    const auto arrow_size = asw::util::get_text_size(f, "<");
    const float y = transform.position.y
        + ((transform.size.y - static_cast<float>(arrow_size.y)) / 2.0F);
    const float inset = padding + 4.0F;

    asw::draw::text(f, "<", { transform.position.x + inset, y }, s.text_hover);
    asw::draw::text(f, ">",
        { transform.position.x + transform.size.x - inset - static_cast<float>(arrow_size.x), y },
        s.text_hover);
}
