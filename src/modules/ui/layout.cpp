#include "./asw/modules/ui/layout.h"

#include <algorithm>
#include <vector>

float asw::ui::detail::ForcedSizes::natural(const Widget& w, float current) const
{
    const auto it = _last.find(w.id());
    if (it != _last.end() && it->second.forced == current) {
        return it->second.natural;
    }
    return current;
}

void asw::ui::detail::ForcedSizes::record(const Widget& w, float natural, float forced)
{
    _next[w.id()] = { .natural = natural, .forced = forced };
}

void asw::ui::detail::ForcedSizes::keep(const Widget& w)
{
    if (const auto it = _last.find(w.id()); it != _last.end()) {
        _next.insert(*it);
    }
}

void asw::ui::detail::ForcedSizes::finish()
{
    _last.swap(_next);
    _next.clear();
}

void asw::ui::Stack::layout(Context& ctx)
{
    const bool vertical = direction == Direction::Vertical;
    const auto& area = transform;

    // Along is the stacking axis, across the other one
    float along = (vertical ? area.position.y : area.position.x) + padding;
    const float across_start = (vertical ? area.position.x : area.position.y) + padding;
    const float across_size = (vertical ? area.size.x : area.size.y) - (padding * 2.0F);

    for (auto const& c : children()) {
        if (!c->visible) {
            _across.keep(*c);
            continue;
        }

        c->measure(ctx);
        auto& t = c->transform;
        float& child_across_size = vertical ? t.size.x : t.size.y;
        const float natural = _across.natural(*c, child_across_size);
        child_across_size = align == Align::Stretch ? across_size : natural;
        _across.record(*c, natural, child_across_size);

        float across = across_start;
        if (align == Align::Center) {
            across += (across_size - child_across_size) / 2.0F;
        } else if (align == Align::End) {
            across += across_size - child_across_size;
        }

        if (vertical) {
            t.position = { across, along };
        } else {
            t.position = { along, across };
        }

        c->layout(ctx);
        along += (vertical ? t.size.y : t.size.x) + gap;
    }

    _across.finish();
}

void asw::ui::Grid::layout(Context& ctx)
{
    const std::size_t cols = std::max<std::size_t>(columns, 1);
    const float inner_w = transform.size.x - (padding * 2.0F);
    const float cell_w
        = std::max(0.0F, (inner_w - (gap * static_cast<float>(cols - 1))) / static_cast<float>(cols));

    std::vector<Widget*> shown;
    std::vector<float> natural_h;
    for (auto const& c : children()) {
        if (c->visible) {
            c->measure(ctx);
            shown.push_back(c.get());
            natural_h.push_back(_heights.natural(*c, c->transform.size.y));
        } else {
            _heights.keep(*c);
        }
    }

    float y = transform.position.y + padding;
    for (std::size_t row = 0; row * cols < shown.size(); ++row) {
        const std::size_t first = row * cols;
        const std::size_t last = std::min(first + cols, shown.size());

        float h = row_height;
        if (h <= 0.0F) {
            for (std::size_t i = first; i < last; ++i) {
                h = std::max(h, natural_h[i]);
            }
        }

        for (std::size_t i = first; i < last; ++i) {
            const auto col = static_cast<float>(i - first);
            auto& t = shown[i]->transform;
            t.position = { transform.position.x + padding + (col * (cell_w + gap)), y };
            t.size = { cell_w, h };
            _heights.record(*shown[i], natural_h[i], h);
            shown[i]->layout(ctx);
        }

        y += h + gap;
    }

    _heights.finish();
}

bool asw::ui::in_row_with_focusables(const Widget& w)
{
    const Widget* parent = w.parent;
    if (parent == nullptr) {
        return false;
    }

    const auto* stack = dynamic_cast<const Stack*>(parent);
    const auto* grid = dynamic_cast<const Grid*>(parent);
    const bool row = (stack != nullptr && stack->direction == Direction::Horizontal)
        || (grid != nullptr && grid->columns > 1);
    if (!row) {
        return false;
    }

    return std::ranges::any_of(parent->children(), [&w](const std::unique_ptr<Widget>& c) {
        return c.get() != &w && c->visible && c->enabled && c->focusable;
    });
}
