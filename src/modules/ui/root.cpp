#include "./asw/modules/ui/root.h"

#include <algorithm>

#include "./asw/modules/action.h"
#include "./asw/modules/display.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/input.h"
#include "./asw/modules/sound.h"
#include "./asw/modules/util.h"

namespace {
void play_ui_sound(const asw::Sample& sample)
{
    if (sample != nullptr) {
        asw::sound::PlayOptions options;
        options.bus = asw::sound::Bus::Ui;
        asw::sound::play(sample, options);
    }
}

// The widget a pointer acts on: the nearest focusable widget at or above
// the hit, so a label inside a button still presses the button
asw::ui::Widget* interactive(asw::ui::Widget* hit)
{
    for (auto* w = hit; w != nullptr; w = w->parent) {
        if (w->focusable) {
            return w;
        }
    }
    return nullptr;
}
} // namespace

asw::ui::Root::Root()
{
    // See through and screen sized, so a UI can sit on top of a scene
    root.bg = { 0, 0, 0, 0 };
    _modals.bg = { 0, 0, 0, 0 };
    fit_to_screen();
}

void asw::ui::Root::fit_to_screen()
{
    const auto screen = asw::display::get_logical_size();
    if (screen.x > 0 && screen.y > 0
        && (root.transform.size.x != static_cast<float>(screen.x)
            || root.transform.size.y != static_cast<float>(screen.y))) {
        root.transform
            = { 0.0F, 0.0F, static_cast<float>(screen.x), static_cast<float>(screen.y) };
        ctx.need_focus_rebuild = true;
    }
    _modals.transform = root.transform;
}

void asw::ui::Root::set_size(float w, float h)
{
    auto r = root.transform;
    r.size.x = w;
    r.size.y = h;
    root.transform = r;
    _modals.transform = r;
    auto_size = false;
    ctx.need_focus_rebuild = true;
}

bool asw::ui::Root::attached(const Widget* w) const
{
    // Removed widgets have no parent, so the walk stops short of root
    while (w != nullptr && w != &root && w != &_modals) {
        w = w->parent;
    }
    return w != nullptr;
}

asw::ui::Modal* asw::ui::Root::top_modal() const
{
    for (auto it = _modals.children().rbegin(); it != _modals.children().rend(); ++it) {
        if ((*it)->visible) {
            return dynamic_cast<Modal*>(it->get());
        }
    }
    return nullptr;
}

asw::ui::Widget& asw::ui::Root::active_root()
{
    if (Modal* modal = top_modal(); modal != nullptr) {
        return *modal;
    }
    return root;
}

bool asw::ui::Root::has_modal() const
{
    return top_modal() != nullptr;
}

void asw::ui::Root::opened_modal()
{
    // Remember focus from before the first modal only
    if (_modals.children().size() == 1) {
        _focus_before_modal = ctx.focus.focused();
    }
    ctx.need_focus_rebuild = true;
}

void asw::ui::Root::close_modals()
{
    while (Modal* modal = top_modal()) {
        modal->close();
    }
    validate();
}

void asw::ui::Root::layout_modals()
{
    for (auto const& c : _modals.children()) {
        if (!c->visible) {
            continue;
        }
        c->measure(ctx);
        auto& t = c->transform;
        t.position = { _modals.transform.position.x + ((_modals.transform.size.x - t.size.x) / 2.0F),
            _modals.transform.position.y + ((_modals.transform.size.y - t.size.y) / 2.0F) };
        c->layout(ctx);
    }
}

void asw::ui::Root::update_modal_focus()
{
    Modal* modal = top_modal();

    // A new top modal: focus its first widget, so a controller can answer it
    if (modal != nullptr && modal != _focused_modal) {
        _focused_modal = modal;
        ctx.focus.focus_start(ctx);
        return;
    }

    // The last modal closed: focus goes back where it was
    if (modal == nullptr && _focused_modal != nullptr) {
        _focused_modal = nullptr;
        Widget* back = _focus_before_modal;
        _focus_before_modal = nullptr;
        if (back != nullptr && attached(back) && back->focusable && back->visible && back->enabled) {
            ctx.focus.set_focus(ctx, back);
        }
    }
}

void asw::ui::Root::validate()
{
    // Widgets can be removed at any time, often from inside a callback. They
    // stay alive until the next update, so a pointer to one can still be read
    // here and dropped.
    if (ctx.hover != nullptr && !attached(ctx.hover)) {
        ctx.hover->_hovered = false;
        ctx.hover = nullptr;
    }
    if (ctx.pointer_capture != nullptr && !attached(ctx.pointer_capture)) {
        ctx.pointer_capture->_pressed = false;
        ctx.pointer_capture->_captured = false;
        ctx.pointer_capture = nullptr;
    }
    if (ctx.focus.default_focus != nullptr && !attached(ctx.focus.default_focus)) {
        ctx.focus.default_focus = nullptr;
    }
    if (_focus_before_modal != nullptr && !attached(_focus_before_modal)) {
        _focus_before_modal = nullptr;
    }

    // Also drops focus from a removed widget, telling it so. While a modal is
    // open only its widgets can have focus.
    ctx.focus.rebuild(ctx, active_root());
    ctx.need_focus_rebuild = false;
    update_modal_focus();
}

void asw::ui::Root::free_removed(Widget& w)
{
    w._removed.clear();
    for (auto const& c : w.children()) {
        free_removed(*c);
    }
}

asw::ui::Widget* asw::ui::Root::hit_test(Widget& w, const asw::Vec2<float>& pointer_pos)
{
    if (!w.visible) {
        return nullptr;
    }

    // traverse children in reverse for top-most
    for (int i = (int)w.children().size() - 1; i >= 0; --i) {
        auto const& c = w.children()[i];
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

bool asw::ui::Root::bubble(Widget* target, const UIEvent& e)
{
    // Take the chain first, a handler that removes its own widget cuts it off
    // from its parent
    std::vector<Widget*> chain;
    for (Widget* w = target; w != nullptr; w = w->parent) {
        chain.push_back(w);
    }

    // Let target handle; if not handled, bubble up to parents. Removed
    // widgets are still alive but take no more events.
    bool handled = false;
    for (Widget* w : chain) {
        if (!attached(w)) {
            continue;
        }
        if (w->on_event(ctx, e)) {
            handled = true;
            break;
        }
    }

    // Handlers may have changed the tree
    validate();
    return handled;
}

bool asw::ui::Root::dispatch_pointer(const UIEvent& e)
{
    Widget* target = nullptr;

    if (ctx.pointer_capture != nullptr) {
        target = ctx.pointer_capture;
    } else {
        target = hit_test(active_root(), e.pointer_pos);
    }

    return bubble(target, e);
}

bool asw::ui::Root::dispatch_to_focused(const UIEvent& e)
{
    Widget* f = ctx.focus.focused();
    if (f == nullptr) {
        return false;
    }
    return bubble(f, e);
}

void asw::ui::Root::activate(Widget& w)
{
    if (w.enabled) {
        play_ui_sound(ctx.theme.sound_activate);
        w.activate(ctx);
        _used = true;
    }

    // The callback may have changed the tree
    validate();
}

void asw::ui::Root::update_pointer()
{
    using namespace asw::input;

    const auto& mouse = get_mouse();
    const bool moved = mouse.change.x != 0.0F || mouse.change.y != 0.0F;
    Widget& layer = active_root();
    Widget* hit = hit_test(layer, mouse.position);
    const bool over_widget = hit != nullptr && hit != &root;

    // Hover every frame, so it is right after a scene change or when widgets
    // move under a still mouse
    Widget* new_hover = interactive(hit);
    if (new_hover != ctx.hover) {
        if (ctx.hover != nullptr) {
            Widget* old_hover = ctx.hover;
            old_hover->_hovered = false;
            ctx.hover = nullptr;
            const UIEvent leave { .type = UIEvent::Type::PointerLeave,
                .pointer_pos = mouse.position };
            old_hover->on_event(ctx, leave);

            // The handler may have changed the tree, new_hover included
            validate();
            if (!attached(new_hover)) {
                new_hover = nullptr;
            }
        }
        ctx.hover = new_hover;
        if (ctx.hover != nullptr) {
            ctx.hover->_hovered = true;
            const UIEvent enter { .type = UIEvent::Type::PointerEnter,
                .pointer_pos = mouse.position };
            ctx.hover->on_event(ctx, enter);

            // The handler may have changed the tree
            validate();
        }
    }

    if (ctx.pointer_capture != nullptr || over_widget) {
        _used = true;
    }

    if (moved) {
        const UIEvent e { .type = UIEvent::Type::PointerMove, .pointer_pos = mouse.position };
        dispatch_pointer(e);
        ctx.show_focus = false;
    }

    // One event per button, carrying the real button so widgets can ignore
    // the ones they do not use
    for (const auto button : { MouseButton::Left, MouseButton::Right, MouseButton::Middle }) {
        const bool left = button == MouseButton::Left;

        if (get_mouse_button_down(button)) {
            ctx.show_focus = false;

            // Press, capture and focus the widget under the pointer
            if (left && ctx.hover != nullptr && ctx.hover->enabled) {
                ctx.pointer_capture = ctx.hover;
                ctx.hover->_pressed = true;
                ctx.hover->_captured = true;
                ctx.focus.set_focus(ctx, ctx.hover);
            }

            const UIEvent e { .type = UIEvent::Type::PointerDown,
                .pointer_pos = mouse.position,
                .mouse_button = button };
            dispatch_pointer(e);
        }

        if (get_mouse_button_up(button)) {
            ctx.show_focus = false;

            const UIEvent e { .type = UIEvent::Type::PointerUp,
                .pointer_pos = mouse.position,
                .mouse_button = button };
            dispatch_pointer(e);

            // Read after the dispatch, a handler may have removed the pressed
            // widget and validate then dropped it, so it is not clicked
            Widget* pressed = left ? ctx.pointer_capture : nullptr;

            // Click: released over the widget that was pressed
            if (pressed != nullptr) {
                const bool over = pressed == ctx.hover;
                pressed->_pressed = false;
                pressed->_captured = false;
                ctx.pointer_capture = nullptr;
                if (over) {
                    activate(*pressed);
                }
            }
        }
    }

    // Pressed look only while the pointer is still over the pressed widget
    if (ctx.pointer_capture != nullptr) {
        ctx.pointer_capture->_pressed = ctx.pointer_capture == ctx.hover;
    }
}

void asw::ui::Root::update_keys()
{
    using namespace asw::input;

    // --- Text Input ---
    bool typed = false;
    if (!input::get_text_input().empty()) {
        const UIEvent ti { .type = UIEvent::Type::TextInput, .text = input::get_text_input() };
        if (dispatch_to_focused(ti)) {
            _used = true;
            typed = true;
        }
    }

    // --- Focus Events ---
    const auto& nav = ctx.navigation;
    Widget* const focused_before = ctx.focus.focused();

    // An action when one is named, else the built in key. Keys that typed
    // text into the focused widget this frame do not also navigate, e.g.
    // Space bound to activate or letters bound to move.
    const auto pressed = [typed](const std::string& action, Key key) {
        if (typed) {
            return false;
        }
        return action.empty() ? get_key_down(key) : get_action_down(action);
    };

    const auto shift = get_key(Key::LShift) || get_key(Key::RShift);

    // Global focus handling first (keyboard-first UX)
    const bool next = pressed(nav.next, Key::Tab);
    const bool prev = !typed && !nav.prev.empty() && get_action_down(nav.prev);
    if (next || prev) {
        if (prev || shift) {
            ctx.focus.focus_prev(ctx);
        } else {
            ctx.focus.focus_next(ctx);
        }

        ctx.show_focus = true;
        _used = true;
    }

    // Directions: dispatch KeyDown to focused widget first, fall back to focus navigation
    const auto direction = [&](const std::string& action, Key key, int dx, int dy) {
        if (!pressed(action, key)) {
            return;
        }
        const UIEvent e { .type = UIEvent::Type::KeyDown, .key = key };
        if (!dispatch_to_focused(e)) {
            ctx.focus.focus_dir(ctx, dx, dy);
        }
        ctx.show_focus = true;
        _used = true;
    };
    direction(nav.up, Key::Up, 0, -1);
    direction(nav.down, Key::Down, 0, +1);
    direction(nav.left, Key::Left, -1, 0);
    direction(nav.right, Key::Right, +1, 0);

    if (ctx.focus.focused() != focused_before) {
        play_ui_sound(ctx.theme.sound_move);
    }

    // Editing keys dispatched to focused widget
    for (const auto key : { Key::Backspace, Key::Delete, Key::Home, Key::End }) {
        if (get_key_down(key)) {
            const UIEvent e { .type = UIEvent::Type::KeyDown, .key = key };
            if (dispatch_to_focused(e)) {
                _used = true;
            }
        }
    }

    // Activate the focused widget. The first press only shows where focus is.
    const bool activate_pressed = nav.activate.empty()
        ? pressed({}, Key::Return) || pressed({}, Key::Space)
        : pressed(nav.activate, Key::Return);
    if (activate_pressed) {
        if (ctx.focus.focus_start(ctx)) {
            ctx.show_focus = true;
            _used = true;
        } else if (Widget* f = ctx.focus.focused(); f != nullptr) {
            ctx.show_focus = true;
            activate(*f);
        }
    }

    // Back goes to the focused widget, then the top modal, then on_back
    if (pressed(nav.back, Key::Escape)) {
        const UIEvent b { .type = UIEvent::Type::Back };
        if (dispatch_to_focused(b)) {
            _used = true;
        } else if (Modal* modal = top_modal(); modal != nullptr) {
            if (modal->close_on_back) {
                modal->close();
            }
            _used = true;
            validate();
        } else if (on_back) {
            on_back();
            _used = true;
            validate();
        }
    }
}

bool asw::ui::Root::update()
{
    _used = false;

    if (auto_size) {
        fit_to_screen();
    }

    // Catch tree changes made since the last update, then free the widgets
    // removed since, nothing points to them any more
    validate();
    free_removed(root);
    free_removed(_modals);

    // Arrange
    root.measure(ctx);
    root.layout(ctx);
    layout_modals();

    update_pointer();
    update_keys();
    update_toasts();

    // A modal takes all input while it is open
    if (has_modal()) {
        _used = true;
    }

    return _used;
}

void asw::ui::Root::toast(const std::string& text)
{
    toast(Toast { .text = text });
}

void asw::ui::Root::toast(Toast message)
{
    _toasts.push_back({ .toast = std::move(message), .age = 0.0F });
}

void asw::ui::Root::clear_toasts()
{
    _toasts.clear();
}

void asw::ui::Root::update_toasts()
{
    const auto now = std::chrono::steady_clock::now();
    float dt = 0.0F;
    if (_last_update) {
        // Capped, so a pause such as a loading screen does not skip messages
        dt = std::min(std::chrono::duration<float>(now - *_last_update).count(), 0.25F);
    }
    _last_update = now;

    const auto& style = ctx.theme.toast;
    std::size_t shown = 0;
    for (auto& t : _toasts) {
        if (shown++ >= style.max_visible) {
            break;
        }
        t.age += dt;
    }

    std::erase_if(_toasts, [&style](const ShownToast& t) {
        return t.age >= (t.toast.seconds > 0.0F ? t.toast.seconds : style.seconds);
    });
}

void asw::ui::Root::draw_toasts()
{
    const auto& style = ctx.theme.toast;
    const auto& font = ctx.theme.font;
    if (font == nullptr) {
        return;
    }

    const auto faded = [](asw::Color c, float alpha) {
        c.a = static_cast<uint8_t>(static_cast<float>(c.a) * alpha);
        return c;
    };

    const auto& area = root.transform;
    const bool from_bottom = style.anchor == Anchor::Bottom || style.anchor == Anchor::BottomLeft
        || style.anchor == Anchor::BottomRight;
    const bool left_side = style.anchor == Anchor::TopLeft || style.anchor == Anchor::BottomLeft;
    const bool right_side = style.anchor == Anchor::TopRight || style.anchor == Anchor::BottomRight;

    float y = from_bottom ? area.position.y + area.size.y - style.margin : area.position.y + style.margin;
    std::size_t shown = 0;
    for (const auto& [t, age] : _toasts) {
        if (shown++ >= style.max_visible) {
            break;
        }

        const float seconds = t.seconds > 0.0F ? t.seconds : style.seconds;
        const float left = seconds - age;
        const float alpha = style.fade_seconds > 0.0F && left < style.fade_seconds
            ? std::max(0.0F, left / style.fade_seconds)
            : 1.0F;

        const auto text_size = asw::util::get_text_size(font, t.text);
        const float text_w = static_cast<float>(text_size.x);
        const float text_h = static_cast<float>(text_size.y);
        const float icon = t.icon != nullptr ? text_h : 0.0F;
        const float icon_gap = t.icon != nullptr ? style.padding / 2.0F : 0.0F;

        const float w = text_w + icon + icon_gap + (style.padding * 2.0F);
        const float h = text_h + (style.padding * 2.0F);
        float x = area.position.x + ((area.size.x - w) / 2.0F);
        if (left_side) {
            x = area.position.x + style.margin;
        } else if (right_side) {
            x = area.position.x + area.size.x - w - style.margin;
        }
        if (from_bottom) {
            y -= h;
        }
        const asw::Quad<float> box { { x, y }, { w, h } };

        if (style.bg.a > 0) {
            asw::draw::rect_fill(box, faded(style.bg, alpha));
        }
        if (style.border_width > 0.0F && style.border.a > 0) {
            asw::draw::rect(box, faded(style.border, alpha), style.border_width);
        }
        if (t.icon != nullptr) {
            asw::draw::stretch_sprite(t.icon, { { x + style.padding, y + style.padding }, { icon, icon } });
        }
        asw::draw::text(font, t.text, { x + style.padding + icon + icon_gap, y + style.padding },
            faded(t.color.value_or(style.text), alpha));

        y = from_bottom ? y - style.gap : y + h + style.gap;
    }
}

void asw::ui::Root::focus(Widget& w, bool show)
{
    ctx.focus.set_focus(ctx, &w);
    ctx.show_focus = show;
}

void asw::ui::Root::clear_focus()
{
    ctx.focus.set_focus(ctx, nullptr);
    ctx.show_focus = false;
}

void asw::ui::Root::draw()
{
    root.draw(ctx);

    // Modals over a dimmed screen
    if (Modal* modal = top_modal(); modal != nullptr) {
        if (const auto dim = modal->get_style(ctx).dim; dim.a > 0) {
            asw::draw::rect_fill(_modals.transform, dim);
        }
        _modals.draw(ctx);
    }

    // Focus ring on top of everything, unless the focused widget was removed
    // since the last update
    Widget* f = ctx.focus.focused();
    if (ctx.show_focus && f != nullptr && f->visible && f->focus_ring && attached(f)) {
        draw_focus_ring(ctx.theme.focus_ring, f->transform);
    }

    // Messages over everything
    draw_toasts();
}
