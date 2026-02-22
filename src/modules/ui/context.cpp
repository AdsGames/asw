#include "./asw/modules/ui/context.h"

#include <cmath>
#include <ranges>

void asw::ui::FocusManager::rebuild(Widget& root)
{
    focusables_.clear();
    dfs(root);
    // Keep current focus if still exists
    if (focused_ != nullptr) {
        auto it = std::ranges::find(focusables_, focused_);
        if (it == focusables_.end()) {
            focused_ = nullptr;
        }
    }
    if (focused_ == nullptr && !focusables_.empty()) {
        focused_ = focusables_.front();
    }
}

void asw::ui::FocusManager::set_focus(Context& ctx, Widget* w)
{
    if (focused_ == w) {
        return;
    }
    if (focused_ != nullptr) {
        focused_->on_focus_changed(ctx, false);
    }
    focused_ = w;
    if (focused_ != nullptr) {
        focused_->on_focus_changed(ctx, true);
    }
}

void asw::ui::FocusManager::focus_next(Context& ctx)
{
    if (focusables_.empty()) {
        return;
    }

    if (focused_ == nullptr) {
        set_focus(ctx, focusables_.front());
        return;
    }
    auto it = std::ranges::find(focusables_, focused_);
    if (it == focusables_.end()) {
        set_focus(ctx, focusables_.front());
        return;
    }
    ++it;
    if (it == focusables_.end()) {
        it = focusables_.begin();
    }
    set_focus(ctx, *it);
}

void asw::ui::FocusManager::focus_prev(Context& ctx)
{
    if (focusables_.empty()) {
        return;
    }

    if (focused_ == nullptr) {
        set_focus(ctx, focusables_.front());
        return;
    }

    auto it = std::ranges::find(focusables_, focused_);
    if (it == focusables_.end()) {
        set_focus(ctx, focusables_.front());
        return;
    }

    if (it == focusables_.begin()) {
        it = focusables_.end();
    }
    --it;
    set_focus(ctx, *it);
}

void asw::ui::FocusManager::focus_dir(Context& ctx, int dx, int dy)
{
    if (focusables_.empty()) {
        return;
    }

    if (focused_ == nullptr) {
        set_focus(ctx, focusables_.front());
        return;
    }

    const auto& from = focused_->transform;
    const float fx = from.get_center().x;
    const float fy = from.get_center().y;

    Widget* best = nullptr;
    float bestScore = 1e30f;

    for (Widget* w : focusables_) {
        if (w == focused_) {
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
        focusables_.push_back(&w);
    }
    for (auto const& c : w.children) {
        dfs(*c);
    }
}
