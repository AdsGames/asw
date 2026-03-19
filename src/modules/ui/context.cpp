#include "./asw/modules/ui/context.h"

#include <cmath>
#include <ranges>

void asw::ui::FocusManager::rebuild(Context& ctx, Widget& root)
{
    _focusables.clear();
    dfs(root);
    // Keep current focus if still exists
    if (_focused != nullptr) {
        auto it = std::ranges::find(_focusables, _focused);
        if (it == _focusables.end()) {
            set_focus(ctx, nullptr);
        }
    }
    if (_focused == nullptr && !_focusables.empty()) {
        set_focus(ctx, _focusables.front());
    }
}

void asw::ui::FocusManager::set_focus(Context& ctx, Widget* w)
{
    if (_focused == w) {
        return;
    }
    if (_focused != nullptr) {
        _focused->on_focus_changed(ctx, false);
    }
    _focused = w;
    if (_focused != nullptr) {
        _focused->on_focus_changed(ctx, true);
    }
}

void asw::ui::FocusManager::focus_next(Context& ctx)
{
    if (_focusables.empty()) {
        return;
    }

    if (_focused == nullptr) {
        set_focus(ctx, _focusables.front());
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

    if (_focused == nullptr) {
        set_focus(ctx, _focusables.front());
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

    if (_focused == nullptr) {
        set_focus(ctx, _focusables.front());
        return;
    }

    const auto& from = _focused->transform;
    const float fx = from.get_center().x;
    const float fy = from.get_center().y;

    Widget* best = nullptr;
    float bestScore = 1e30f;

    for (Widget* w : _focusables) {
        if (w == _focused) {
            continue;
        }
        const auto& to = w->transform;
        const float tx = to.get_center().x;
        const float ty = to.get_center().y;
        const float vx = tx - fx;
        const float vy = ty - fy;

        // Must be in the desired half-plane
        if (dx != 0 && (vx * dx) <= 0.0f) {
            continue;
        }
        if (dy != 0 && (vy * dy) <= 0.0f) {
            continue;
        }

        // Score: prefer small primary-axis distance, penalize orthogonal
        // distance.
        const float primary = (dx != 0) ? std::abs(vx) : std::abs(vy);
        const float ortho = (dx != 0) ? std::abs(vy) : std::abs(vx);

        // Add small bias for actual Euclidean distance to break ties
        const float dist2 = (vx * vx) + (vy * vy);

        const float score = (primary * 1.0f) + (ortho * 2.0f) + (dist2 * 0.001f);
        if (score < bestScore) {
            bestScore = score;
            best = w;
        }
    }

    if (best != nullptr) {
        set_focus(ctx, best);
    }
}

void asw::ui::FocusManager::dfs(Widget& w)
{
    if (w.visible && w.enabled && w.focusable) {
        _focusables.push_back(&w);
    }
    for (auto const& c : w.children) {
        dfs(*c);
    }
}
