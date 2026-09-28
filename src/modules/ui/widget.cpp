#include "./asw/modules/ui/widget.h"

#include "./asw/modules/ui/context.h"

void asw::ui::Widget::layout(Context& ctx)
{
    (void)ctx;
    for (auto const& c : children) {
        c->layout(ctx);
    }
}

bool asw::ui::Widget::on_event(Context& ctx, const UIEvent& e)
{
    (void)ctx;
    (void)e;
    return false;
}

void asw::ui::Widget::on_focus_changed(Context& ctx, bool focused)
{
    (void)ctx;
    (void)focused;
}

void asw::ui::Widget::activate(Context& ctx)
{
    (void)ctx;
}

bool asw::ui::Widget::is_highlighted(const Context& ctx) const
{
    return _pressed || _hovered || (_focused && ctx.show_focus);
}

void asw::ui::Widget::draw(Context& ctx)
{
    for (auto const& c : children) {
        if (c->visible) {
            c->draw(ctx);
        }
    }
}
