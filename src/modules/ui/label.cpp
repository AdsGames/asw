#include "./asw/modules/ui/label.h"

#include "./asw/modules/draw.h"
#include "./asw/modules/util.h"

void asw::ui::Label::set_text(const std::string& t, bool auto_size)
{
    text = t;
    _fit_text = auto_size;
}

void asw::ui::Label::measure(Context& ctx)
{
    if (!_fit_text) {
        return;
    }
    const auto size = asw::util::get_text_size(pick_font(font, ctx.theme), text);
    transform.size = { static_cast<float>(size.x), static_cast<float>(size.y) };
}

void asw::ui::Label::draw(Context& ctx)
{
    if (const auto& f = pick_font(font, ctx.theme); !text.empty() && f != nullptr) {
        // Text is drawn from its left, middle or right, so a sized label
        // lines its text up inside its own box
        auto position = transform.position;
        if (_fit_text || transform.size.x > 0.0F) {
            if (justify == asw::TextJustify::Center) {
                position.x += transform.size.x / 2.0F;
            } else if (justify == asw::TextJustify::Right) {
                position.x += transform.size.x;
            }
        }
        asw::draw::text(f, text, position, color.value_or(ctx.theme.text), justify);
    }

    Widget::draw(ctx);
}
