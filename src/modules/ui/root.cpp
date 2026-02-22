#include "./asw/modules/ui/root.h"

#include "./asw/modules/input.h"

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
    ctx.focus.rebuild(ctx, root);
    ctx.hover = nullptr;
    ctx.pointer_capture = nullptr;
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
    for (Widget* w = target; w != nullptr; w = w->parent) {
        if (w->on_event(ctx, e)) {
            return true;
        }
    }
    return false;
}

bool asw::ui::Root::dispatch_to_focused(const UIEvent& e)
{
    Widget* f = ctx.focus.focused();
    if (f == nullptr) {
        return false;
    }
    for (Widget* w = f; w != nullptr; w = w->parent) {
        if (w->on_event(ctx, e)) {
            return true;
        }
    }
    return false;
}

void asw::ui::Root::update()
{
    using namespace asw::input;

    // Rebuild focus list if needed
    rebuild_focus_if_needed();

    // Arrange
    root.layout(ctx);

    // --- Mouse ---

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

    // Button Down
    if (get_mouse_button_down(MouseButton::Left) || get_mouse_button_down(MouseButton::Right)
        || get_mouse_button_down(MouseButton::Middle)) {
        const UIEvent e { .type = UIEvent::Type::PointerDown,
            .pointer_pos = mouse.position,
            .mouse_button = MouseButton::Left };
        dispatch_pointer(e);
        ctx.theme.show_focus = false;
    }

    // Button Up
    if (get_mouse_button_up(MouseButton::Left) || get_mouse_button_up(MouseButton::Right)
        || get_mouse_button_up(MouseButton::Middle)) {
        const UIEvent e { .type = UIEvent::Type::PointerUp,
            .pointer_pos = mouse.position,
            .mouse_button = MouseButton::Left };
        dispatch_pointer(e);
        ctx.theme.show_focus = false;
    }

    // --- Text Input ---
    if (!input::text_input.empty()) {
        const UIEvent ti { .type = UIEvent::Type::TextInput, .text = input::text_input };
        dispatch_to_focused(ti);
    }

    // --- Focus Events ---
    const auto shift = get_key(Key::LShift) || get_key(Key::RShift);

    // Global focus handling first (keyboard-first UX)
    if (get_key_down(Key::Tab)) {
        if (shift) {
            ctx.focus.focus_prev(ctx);
        } else {
            ctx.focus.focus_next(ctx);
        }

        ctx.theme.show_focus = true;
    }

    // Arrow keys: dispatch KeyDown to focused widget first, fall back to focus navigation
    if (get_key_down(Key::Up)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Up, .shift = shift };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, 0, -1);
            ctx.theme.show_focus = true;
        }
    }
    if (get_key_down(Key::Down)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Down, .shift = shift };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, 0, +1);
            ctx.theme.show_focus = true;
        }
    }
    if (get_key_down(Key::Left)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Left, .shift = shift };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, -1, 0);
            ctx.theme.show_focus = true;
        }
    }
    if (get_key_down(Key::Right)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Right, .shift = shift };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, +1, 0);
            ctx.theme.show_focus = true;
        }
    }

    // Editing keys dispatched to focused widget
    if (get_key_down(Key::Backspace)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Backspace, .shift = shift };
        dispatch_to_focused(e);
    }
    if (get_key_down(Key::Delete)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Delete, .shift = shift };
        dispatch_to_focused(e);
    }
    if (get_key_down(Key::Home)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::Home, .shift = shift };
        dispatch_to_focused(e);
    }
    if (get_key_down(Key::End)) {
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = Key::End, .shift = shift };
        dispatch_to_focused(e);
    }

    // Activate/back routed to focused widget
    if (get_key_down(Key::Return) || get_key_down(Key::Space)) {
        const UIEvent a { .type = UIEvent::Type::Activate };
        dispatch_to_focused(a);
    }
    if (get_key_down(Key::Escape)) {
        const UIEvent b { .type = UIEvent::Type::Back };
        dispatch_to_focused(b);
    }
}

void asw::ui::Root::draw()
{
    root.draw(ctx);
}
