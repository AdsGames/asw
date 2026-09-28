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
};

} // namespace asw::ui

#endif // ASW_MODULES_UI_LAYOUT_H
