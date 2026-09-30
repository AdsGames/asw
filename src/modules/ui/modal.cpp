#include "./asw/modules/ui/modal.h"

#include <algorithm>

#include "./asw/modules/draw.h"
#include "./asw/modules/ui/context.h"

asw::ui::Modal::Modal()
{
    direction = Direction::Vertical;
    align = Align::Center;
}

const asw::ui::ModalStyle& asw::ui::Modal::get_style(const Context& ctx) const
{
    return style ? *style : ctx.theme.modal;
}

asw::ui::Label& asw::ui::Modal::add_text(const std::string& text, const asw::Font& font)
{
    auto& label = add_child<Label>();
    label.font = font;
    label.justify = asw::TextJustify::Center;
    label.set_text(text, true);
    return label;
}

asw::ui::Button& asw::ui::Modal::add_button(const std::string& text, std::function<void()> on_click)
{
    auto& button = add_child<Button>();
    button.padding = 8.0F;
    button.set_text(text, true);
    button.on_click = std::move(on_click);
    return button;
}

void asw::ui::Modal::close()
{
    if (_closed) {
        return;
    }
    _closed = true;

    if (on_close) {
        on_close();
    }

    // Kept alive by the parent until Root frees it
    if (parent != nullptr) {
        parent->remove_child(*this);
    }
}

void asw::ui::Modal::measure(Context& ctx)
{
    const auto& s = get_style(ctx);
    padding = s.padding;
    gap = s.gap;

    float width = 0.0F;
    float height = 0.0F;
    int shown = 0;
    for (auto const& c : children()) {
        if (!c->visible) {
            continue;
        }
        c->measure(ctx);
        width = std::max(width, c->transform.size.x);
        height += c->transform.size.y;
        shown += 1;
    }
    if (shown > 1) {
        height += gap * static_cast<float>(shown - 1);
    }

    transform.size = { std::max(s.min_width, width + (padding * 2.0F)), height + (padding * 2.0F) };
}

bool asw::ui::Modal::on_event(Context& ctx, const UIEvent& e)
{
    if (e.type == UIEvent::Type::Back && close_on_back) {
        close();
        return true;
    }
    return Stack::on_event(ctx, e);
}

void asw::ui::Modal::draw(Context& ctx)
{
    const auto& s = get_style(ctx);
    if (s.bg.a > 0) {
        asw::draw::rect_fill(transform, s.bg);
    }
    if (s.border_width > 0.0F && s.border.a > 0) {
        asw::draw::rect(transform, s.border, s.border_width);
    }

    Stack::draw(ctx);
}
