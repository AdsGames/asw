/// @file display.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Display and window routines for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_DISPLAY_H
#define ASW_DISPLAY_H

#include <string>

#include "./color.h"
#include "./geometry.h"
#include "./types.h"

namespace asw::display {

/// @brief Initialize the display module. Called by asw::core::init().
///
/// @param width The logical width of the display.
/// @param height The logical height of the display.
/// @param scale The initial window scale factor.
///
void _init(int width, int height, int scale);

/// @brief Initialize the display module with OpenGL support. Called by asw::core::init().
///
/// @param width The logical width of the display.
/// @param height The logical height of the display.
/// @param scale The initial window scale factor.
///
void _init_opengl(int width, int height, int scale);

/// @brief Shut down the display module. Called by asw::core::shutdown().
/// Nulls the renderer and window pointers before destroying them, so any
/// outstanding shared_ptr asset deleters see a null renderer and become no-ops.
///
void _shutdown();

/// @brief Get the SDL renderer.
///
/// @return Pointer to the SDL_Renderer, or nullptr if not initialized.
///
asw::Renderer* get_renderer();

/// @brief Get the SDL window.
///
/// @return Pointer to the SDL_Window, or nullptr if not initialized.
///
asw::Window* get_window();

/// @brief Set the title of the window.
///
/// @param title The title to display at the top of the window.
///
void set_title(const std::string& title);

/// @brief Set the icon to display on the window. If it does not exist, this
/// will silently fail.
///
/// @param path A path to the icon to display.
///
void set_icon(const std::string& path);

/// @brief Set the window to fullscreen or windowed.
///
/// @details When the config sets display.fullscreen, it wins and
/// @p fullscreen is ignored.
///
/// @param fullscreen Whether or not to set the window to fullscreen.
///
void set_fullscreen(bool fullscreen);

/// @brief Check if the window is fullscreen.
///
/// @details Reads the window itself, so it is right when the config sets
/// display.fullscreen. Useful for showing the real state in a settings menu.
///
/// @return true if the window is fullscreen.
///
bool is_fullscreen();

/// @brief Set the resolution of the window.
///
/// @param w The width of the window.
/// @param h The height of the window.
///
void set_resolution(int w, int h);

/// @brief Set resizable flag of the window.
///
/// @param resizable Whether or not the window is resizable.
///
void set_resizable(bool resizable);

/// @brief Get the size of the window.
///
/// @return The size of the window.
///
asw::Vec2<int> get_size();

/// @brief Get the logical size of the window. This may differ from the
/// actual size if scaling is enabled.
///
/// @return The logical size of the window.
///
asw::Vec2<int> get_logical_size();

/// @brief Get how many output pixels one logical pixel covers, after
/// letterboxing. 2.0 means the game is drawn at twice its logical size.
///
/// @return The scale of the window.
///
asw::Vec2<float> get_scale();

/// @brief Set the render target to the window.
///
void set_render_target(const asw::Texture& texture);

/// @brief Reset the render target to the default.
///
void reset_render_target();

/// @brief Clear the window to black.
///
void clear();

/// @brief Clear to colour
///
/// @param color The color to clear to.
///
void clear(const asw::Color& color);

/// @brief Present the window.
///
void present();

/// @brief Set the blend mode used to draw primitives such as rectangles,
/// lines and circles. Defaults to Blend, so colours with alpha are see
/// through.
///
/// @param mode The blend mode to set.
///
void set_blend_mode(asw::BlendMode mode);

/// @brief Save what has been drawn this frame to a PNG file. Call after
/// drawing and before the frame is presented, for example at the end of a
/// scene's draw(). Works with the dummy video driver
/// (SDL_VIDEO_DRIVER=dummy), so scripted runs can capture frames without a
/// window on screen.
///
/// @param path Where to write the PNG.
/// @return true - If the file was written.
/// @return false - If reading the frame or writing the file failed.
///
bool screenshot(const std::string& path);

/// @brief Warp mouse in window
///
/// @param x The x coordinate to warp to.
/// @param y The y coordinate to warp to.
///
void warp_mouse(float x, float y);

/// @brief Swap window (gl)
///
void swap_window();

} // namespace asw::display

#endif // ASW_DISPLAY_H