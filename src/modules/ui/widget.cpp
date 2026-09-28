#include "./asw/modules/ui/widget.h"

#include <algorithm>
#include <utility>

#include "./asw/modules/ui/context.h"

asw::ui::Widget::Widget(Widget&& other) noexcept
    : visible(other.visible)
    , enabled(other.enabled)
    , focusable(other.focusable)
    , nav_up(other.nav_up)
    , nav_down(other.nav_down)
    , nav_left(other.nav_left)
    , nav_right(other.nav_right)
    , focus_ring(other.focus_ring)
    , transform(other.transform)
    , _children(std::move(other._children))
    , _id(generate_id())
{
    adopt_children();
}

asw::ui::Widget& asw::ui::Widget::operator=(Widget&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    visible = other.visible;
    enabled = other.enabled;
    focusable = other.focusable;
    nav_up = other.nav_up;
    nav_down = other.nav_down;
    nav_left = other.nav_left;
    nav_right = other.nav_right;
    focus_ring = other.focus_ring;
    transform = other.transform;

    // Root may still point into the old children, keep them until it frees them
    detach_children();
    _children = std::move(other._children);

    adopt_children();
    return *this;
}

void asw::ui::Widget::adopt_children()
{
    for (auto const& c : _children) {
        c->parent = this;
    }
}

void asw::ui::Widget::detach_children()
{
    for (auto& c : _children) {
        c->parent = nullptr;
        _removed.push_back(std::move(c));
    }
    _children.clear();
}

void asw::ui::Widget::measure(Context& ctx)
{
    (void)ctx;
}

void asw::ui::Widget::layout(Context& ctx)
{
    for (auto const& c : _children) {
        c->measure(ctx);
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
    for (auto const& c : _children) {
        if (c->visible) {
            c->draw(ctx);
        }
    }
}

bool asw::ui::Widget::remove_child(const Widget& child)
{
    const auto it = std::ranges::find_if(
        _children, [&child](const std::unique_ptr<Widget>& c) { return c.get() == &child; });
    if (it == _children.end()) {
        return false;
    }
    (*it)->parent = nullptr;
    _removed.push_back(std::move(*it));
    _children.erase(it);
    return true;
}

void asw::ui::Widget::clear_children()
{
    detach_children();
}
