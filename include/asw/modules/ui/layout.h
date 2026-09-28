/// @file layout.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Layout widgets for the ASW UI module
/// @date 2026-09-28
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_MODULES_UI_LAYOUT_H
#define ASW_MODULES_UI_LAYOUT_H

#include <cstddef>
#include <unordered_map>

#include "context.h"
#include "widget.h"

namespace asw::ui {

/// @brief Stack direction.
///
enum class Direction {
    Vertical,
    Horizontal,
};

/// @brief Where children sit across the stack: left to right for a vertical
/// stack, top to bottom for a horizontal one.
///
enum class Align {
    Start,
    Center,
    End,
    /// Children fill the stack across.
    Stretch,
};

namespace detail {

/// @brief Sizes a layout forced on its children, so a child gets its own size
/// back when the layout stops forcing it, e.g. a grid row after its tallest
/// child is hidden.
///
class ForcedSizes {
public:
    /// @brief The child's own size: its current size, unless that is still
    /// the size this layout forced on it last time.
    ///
    /// @param w The child.
    /// @param current The child's current size.
    /// @return The child's own size.
    ///
    float natural(const Widget& w, float current) const;

    /// @brief Record the size forced on a child this layout.
    ///
    /// @param w The child.
    /// @param natural The child's own size.
    /// @param forced The size the layout gave it.
    ///
    void record(const Widget& w, float natural, float forced);

    /// @brief Keep last layout's record for a child skipped this layout,
    /// e.g. a hidden one.
    ///
    /// @param w The child.
    ///
    void keep(const Widget& w);

    /// @brief End the layout. Records not made or kept this layout are dropped.
    ///
    void finish();

private:
    struct Entry {
        float natural;
        float forced;
    };

    std::unordered_map<WidgetId, Entry> _last;
    std::unordered_map<WidgetId, Entry> _next;
};

} // namespace detail

/// @brief Places children one after another, top to bottom or left to right.
///
/// @details Children keep their own size along the stack, for example the
/// height of each row in a vertical stack. Across the stack they follow align.
/// Hidden children take no space. Nest stacks for rows inside columns.
///
class Stack : public Widget {
public:
    /// @brief Direction children are placed in.
    Direction direction = Direction::Vertical;

    /// @brief Where children sit across the stack.
    Align align = Align::Stretch;

    /// @brief Space between children.
    float gap = 8.0F;

    /// @brief Space inside the stack on every side.
    float padding = 0.0F;

    /// @brief Place the children.
    ///
    /// @param ctx The UI context.
    ///
    void layout(Context& ctx) override;

private:
    // Sizes across the stack forced by Align::Stretch
    detail::ForcedSizes _across;
};

/// @brief A vertical stack with padding, for a column of rows.
///
class VBox : public Stack {
public:
    /// @brief Default constructor.
    ///
    VBox()
    {
        padding = 10.0F;
    }
};

/// @brief Places children in a grid, filling each row left to right, e.g. a
/// level select or a 2 by 2 menu.
///
/// @details Every cell is the same width, set by the columns. A row is
/// row_height tall, or as tall as its tallest child when row_height is 0.
/// Children fill their cell. Hidden children take no cell.
///
class Grid : public Widget {
public:
    /// @brief Number of columns.
    std::size_t columns = 2;

    /// @brief Space between cells, both ways.
    float gap = 8.0F;

    /// @brief Space inside the grid on every side.
    float padding = 0.0F;

    /// @brief Height of every row, 0 for the tallest child in the row.
    float row_height = 0.0F;

    /// @brief Place the children.
    ///
    /// @param ctx The UI context.
    ///
    void layout(Context& ctx) override;

private:
    // Heights forced by the row height
    detail::ForcedSizes _heights;
};

/// @brief Whether a widget sits in a row with other widgets that can take
/// focus: its parent is a horizontal Stack, or a Grid with more than one
/// column, holding another visible, enabled, focusable child.
///
/// @details Value widgets (Choice, Slider) use this to leave left and right
/// for moving between the widgets in the row.
///
/// @param w The widget.
/// @return True if left and right should move focus within the row.
///
bool in_row_with_focusables(const Widget& w);

} // namespace asw::ui

#endif // ASW_MODULES_UI_LAYOUT_H
