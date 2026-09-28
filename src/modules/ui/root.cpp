#include "./asw/modules/ui/root.h"

#include <algorithm>

#include "./asw/modules/action.h"
#include "./asw/modules/input.h"

namespace {
void collect(asw::ui::Widget& w, std::vector<asw::ui::Widget*>& out)
{
    out.push_back(&w);
    for (auto const& c : w.children) {
        collect(*c, out);
    }
}

bool in_tree(const std::vector<asw::ui::Widget*>& live, const asw::ui::Widget* w)
{
    return w == nullptr || std::ranges::find(live, w) != live.end();
}
} // namespace

asw::ui::Root::Root()
{
    root.transform = { 0, 0, 128, 128 };
    root.bg = ctx.theme.panel_bg;
}

void asw::ui::Root::set_size(float w, float h)
{
    auto r = root.transform;
    r.size.x = w;
    r.size.y = h;
    root.transform = r;
    ctx.need_focus_rebuild = true;
}

void asw::ui::Root::rebuild_focus_if_needed()
{
    if (!ctx.need_focus_rebuild) {
        return;
    }
    validate();
}

void asw::ui::Root::validate()
{
    // Widgets can be removed at any time, often from inside a callback, so
    // drop any pointer that no longer points into the tree before using it
    _live.clear();
    collect(root, _live);

    if (!in_tree(_live, ctx.hover)) {
        ctx.hover = nullptr;
    }
    if (!in_tree(_live, ctx.pointer_capture)) {
        ctx.pointer_capture = nullptr;
    }
    if (!in_tree(_live, ctx.focus.focused())) {
        ctx.focus.forget_focus();
    }

    ctx.focus.rebuild(ctx, root);
    ctx.need_focus_rebuild = false;
}

asw::ui::Widget* asw::ui::Root::hit_test(Widget& w, const asw::Vec2<float>& pointer_pos)
{
    if (!w.visible) {
        return nullptr;
    }

    // traverse children in reverse for top-most
    for (int i = (int)w.children.size() - 1; i >= 0; --i) {
        auto const& c = w.children[i];
        if (!c->visible) {
            continue;
        }
        if (!c->transform.contains(pointer_pos)) {
            continue;
        }
        if (auto* hit = hit_test(*c, pointer_pos)) {
            return hit;
        }
        return c.get();
    }
    return w.transform.contains(pointer_pos) ? &w : nullptr;
}

bool asw::ui::Root::dispatch_pointer(const UIEvent& e)
{
    Widget* target = nullptr;

    if (ctx.pointer_capture != nullptr) {
        target = ctx.pointer_capture;
    } else {
        target = hit_test(root, e.pointer_pos);
    }

    // Let target handle; if not handled, bubble up to parents
    bool handled = false;
    for (Widget* w = target; w != nullptr; w = w->parent) {
        if (w->on_event(ctx, e)) {
            handled = true;
            break;
        }
    }

    // Handlers may have changed the tree
    validate();
    return handled;
}

bool asw::ui::Root::dispatch_to_focused(const UIEvent& e)
{
    Widget* f = ctx.focus.focused();
    if (f == nullptr) {
        return false;
    }
    bool handled = false;
    for (Widget* w = f; w != nullptr; w = w->parent) {
        if (w->on_event(ctx, e)) {
            handled = true;
            break;
        }
    }

    // Handlers may have changed the tree
    validate();
    return handled;
}

void asw::ui::Root::update()
{
    using namespace asw::input;

    // Catch tree changes made since the last update
    validate();

    // Arrange
    root.layout(ctx);

    // --- Mouse ---
    const auto& mouse = get_mouse();

    // Hover and Unhover events
    if (mouse.change.x != 0.0F || mouse.change.y != 0.0F) {
        // Send leave/enter when the hovered widget changes
        if (Widget* new_hover = hit_test(root, mouse.position); new_hover != ctx.hover) {
            if (ctx.hover != nullptr) {
                const UIEvent leave { .type = UIEvent::Type::PointerLeave,
                    .pointer_pos = mouse.position };
                ctx.hover->on_event(ctx, leave);
            }
            ctx.hover = new_hover;
            if (ctx.hover != nullptr) {
                const UIEvent enter { .type = UIEvent::Type::PointerEnter,
                    .pointer_pos = mouse.position };
                ctx.hover->on_event(ctx, enter);
            }
        }

        // Also dispatch the regular move event
        const UIEvent e { .type = UIEvent::Type::PointerMove, .pointer_pos = mouse.position };
        dispatch_pointer(e);
        ctx.theme.show_focus = false;
    }

    // --- Button events ---

    // One event per button, carrying the real button so widgets can ignore
    // the ones they do not use
    for (const auto button : { MouseButton::Left, MouseButton::Right, MouseButton::Middle }) {
        if (get_mouse_button_down(button)) {
            const UIEvent e { .type = UIEvent::Type::PointerDown,
                .pointer_pos = mouse.position,
                .mouse_button = button };
            dispatch_pointer(e);
            ctx.theme.show_focus = false;
        }
        if (get_mouse_button_up(button)) {
            const UIEvent e { .type = UIEvent::Type::PointerUp,
                .pointer_pos = mouse.position,
                .mouse_button = button };
            dispatch_pointer(e);
            ctx.theme.show_focus = false;
        }
    }

    // --- Text Input ---
    if (!input::get_text_input().empty()) {
        const UIEvent ti { .type = UIEvent::Type::TextInput, .text = input::get_text_input() };
        dispatch_to_focused(ti);
    }

    // --- Focus Events ---
    const auto& nav = ctx.navigation;

    // An action when one is named, else the built in key
    const auto pressed = [](const std::string& action, Key key) {
        return action.empty() ? get_key_down(key) : get_action_down(action);
    };

    const auto shift = get_key(Key::LShift) || get_key(Key::RShift);

    // Global focus handling first (keyboard-first UX)
    const bool next = pressed(nav.next, Key::Tab);
    const bool prev = nav.prev.empty() ? false : get_action_down(nav.prev);
    if (next || prev) {
        if (prev || shift) {
            ctx.focus.focus_prev(ctx);
        } else {
            ctx.focus.focus_next(ctx);
        }

        ctx.theme.show_focus = true;
    }

    // Directions: dispatch KeyDown to focused widget first, fall back to focus navigation
    const auto direction = [&](const std::string& action, Key key, int dx, int dy) {
        if (!pressed(action, key)) {
            return;
        }
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = key };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, dx, dy);
            ctx.theme.show_focus = true;
        }
    };
    direction(nav.up, Key::Up, 0, -1);
    direction(nav.down, Key::Down, 0, +1);
    direction(nav.left, Key::Left, -1, 0);
    direction(nav.right, Key::Right, +1, 0);

    // Editing keys dispatched to focused widget
    for (const auto key : { Key::Backspace, Key::Delete, Key::Home, Key::End }) {
        if (get_key_down(key)) {
            const UIEvent e { .type = UIEvent::Type::KeyDown, .key = key };
            dispatch_to_focused(e);
        }
    }

    // Activate/back routed to focused widget
    const bool activate = nav.activate.empty()
        ? get_key_down(Key::Return) || get_key_down(Key::Space)
        : get_action_down(nav.activate);
    if (activate) {
        const UIEvent a { .type = UIEvent::Type::Activate };
        dispatch_to_focused(a);
    }
    if (pressed(nav.back, Key::Escape)) {
        const UIEvent b { .type = UIEvent::Type::Back };
        if (!dispatch_to_focused(b) && on_back) {
            on_back();
        }
    }
}

void asw::ui::Root::draw()
{
    root.draw(ctx);
}
