#include "./asw/modules/ui/context.h"

#include <algorithm>
#include <cmath>
#include <ranges>

void asw::ui::FocusManager::rebuild(Context& ctx, Widget& root)
{
    _focusables.clear();
    dfs(root);
    // Keep current focus if still exists. Nothing is focused until the
    // player navigates or clicks, so no widget looks selected by default.
    if (_focused != nullptr) {
        auto it = std::ranges::find(_focusables, _focused);
        if (it == _focusables.end()) {
            set_focus(ctx, nullptr);
        }
    }
}

bool asw::ui::FocusManager::focus_start(Context& ctx)
{
    if (_focused != nullptr || _focusables.empty()) {
        return false;
    }

    const bool has_default = default_focus != nullptr
        && std::ranges::find(_focusables, default_focus) != _focusables.end();
    set_focus(ctx, has_default ? default_focus : _focusables.front());
    return true;
}

void asw::ui::FocusManager::set_focus(Context& ctx, Widget* w)
{
    _lane_x.reset();
    _lane_y.reset();

    if (_focused == w) {
        return;
    }
    if (_focused != nullptr) {
        _focused->_focused = false;
        _focused->on_focus_changed(ctx, false);
    }
    _focused = w;
    if (_focused != nullptr) {
        _focused->_focused = true;
        _focused->on_focus_changed(ctx, true);
    }
}

void asw::ui::FocusManager::focus_next(Context& ctx)
{
    if (_focusables.empty()) {
        return;
    }

    if (focus_start(ctx)) {
        return;
    }
    auto it = std::ranges::find(_focusables, _focused);
    if (it == _focusables.end()) {
        set_focus(ctx, _focusables.front());
        return;
    }
    ++it;
    if (it == _focusables.end()) {
        it = _focusables.begin();
    }
    set_focus(ctx, *it);
}

void asw::ui::FocusManager::focus_prev(Context& ctx)
{
    if (_focusables.empty()) {
        return;
    }

    if (focus_start(ctx)) {
        return;
    }

    auto it = std::ranges::find(_focusables, _focused);
    if (it == _focusables.end()) {
        set_focus(ctx, _focusables.front());
        return;
    }

    if (it == _focusables.begin()) {
        it = _focusables.end();
    }
    --it;
    set_focus(ctx, *it);
}

void asw::ui::FocusManager::focus_dir(Context& ctx, int dx, int dy)
{
    if (_focusables.empty()) {
        return;
    }

    if (focus_start(ctx)) {
        return;
    }

    Widget* const from_widget = _focused;
    const bool vertical = dy != 0;
    const int dir = vertical ? dy : dx;

    // An explicit neighbour wins, if it can take focus
    Widget* override_target = nullptr;
    if (dy < 0) {
        override_target = from_widget->nav_up;
    } else if (dy > 0) {
        override_target = from_widget->nav_down;
    } else if (dx < 0) {
        override_target = from_widget->nav_left;
    } else if (dx > 0) {
        override_target = from_widget->nav_right;
    }
    if (override_target != nullptr
        && std::ranges::find(_focusables, override_target) != _focusables.end()) {
        set_focus(ctx, override_target);
        return;
    }

    // Along is the direction of travel, across the other axis
    const auto along_lo = [vertical](const asw::Quad<float>& q) {
        return vertical ? q.position.y : q.position.x;
    };
    const auto along_hi = [vertical](const asw::Quad<float>& q) {
        return vertical ? q.position.y + q.size.y : q.position.x + q.size.x;
    };
    const auto across_lo = [vertical](const asw::Quad<float>& q) {
        return vertical ? q.position.x : q.position.y;
    };
    const auto across_hi = [vertical](const asw::Quad<float>& q) {
        return vertical ? q.position.x + q.size.x : q.position.y + q.size.y;
    };

    const auto& from = from_widget->transform;

    // Repeated presses one way keep to the lane they started in, so down then
    // up comes back to the same column
    std::optional<float>& lane = vertical ? _lane_x : _lane_y;
    const float anchor = lane.value_or((across_lo(from) + across_hi(from)) / 2.0F);

    Widget* best = nullptr;
    bool best_in_lane = false;
    float best_distance = 0.0F;
    float best_offset = 0.0F;

    for (Widget* w : _focusables) {
        if (w == from_widget) {
            continue;
        }
        const auto& to = w->transform;

        // Its centre must lie past the edge we leave from, so a wide widget
        // does not move sideways into the row below it
        const float to_center = (along_lo(to) + along_hi(to)) / 2.0F;
        const float leading_edge = dir > 0 ? along_hi(from) : along_lo(from);
        if ((to_center - leading_edge) * static_cast<float>(dir) <= 0.0F) {
            continue;
        }

        // Edge to edge gap along the direction, 0 when they overlap
        const float gap = dir > 0 ? along_lo(to) - along_hi(from) : along_lo(from) - along_hi(to);
        const float along = std::max(0.0F, gap);

        // Sharing a lane: the ranges across the direction overlap
        const float overlap = std::min(across_hi(from), across_hi(to))
            - std::max(across_lo(from), across_lo(to));
        const bool in_lane = overlap > 0.0F;
        const float across_gap = in_lane ? 0.0F : -overlap;

        // Out of lane widgets pay for how far off to the side they are
        const float distance = in_lane ? along : along + (across_gap * 2.0F);
        const float offset = std::abs(((across_lo(to) + across_hi(to)) / 2.0F) - anchor);

        const bool better = best == nullptr || (in_lane && !best_in_lane)
            || (in_lane == best_in_lane
                && (distance < best_distance
                    || (distance == best_distance && offset < best_offset)));
        if (better) {
            best = w;
            best_in_lane = in_lane;
            best_distance = distance;
            best_offset = offset;
        }
    }

    if (best != nullptr) {
        set_focus(ctx, best);
        lane = anchor;
    }
}

void asw::ui::FocusManager::dfs(Widget& w)
{
    // A hidden or disabled widget hides or disables its children too
    if (!w.visible || !w.enabled) {
        return;
    }
    if (w.focusable) {
        _focusables.push_back(&w);
    }
    for (auto const& c : w.children()) {
        dfs(*c);
    }
}
