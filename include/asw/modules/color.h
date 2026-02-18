#ifndef ASW_COLOR_H
#define ASW_COLOR_H

#include <SDL3/SDL.h>

#include <string>

namespace asw {
  /// @brief RGBA color struct with 8-bit channels.
  ///
  struct Color {
    uint8_t r, g, b, a;

    /// @brief Construct a default Color (black, fully opaque).
    Color() : r(0), g(0), b(0), a(255) {}

    /// @brief Construct a Color from RGBA values.
    ///
    /// @param r Red channel (0-255).
    /// @param g Green channel (0-255).
    /// @param b Blue channel (0-255).
    /// @param a Alpha channel (0-255, default 255).
    ///
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    /// @brief Convert to an SDL_Color.
    ///
    /// @return The equivalent SDL_Color.
    ///
    SDL_Color toSDLColor() const { return SDL_Color{r, g, b, a}; }

    /// @brief From float RGBA values (0.0-1.0) to Color.
    ///
    /// @param r Red channel (0.0-1.0).
    /// @param g Green channel (0.0-1.0).
    /// @param b Blue channel (0.0-1.0).
    /// @param a Alpha channel (0.0-1.0, default 1.0).
    /// @return The equivalent Color.
    ///
    static Color fromFloat(float r, float g, float b, float a = 1.0F) {
      return {static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255),
              static_cast<uint8_t>(b * 255), static_cast<uint8_t>(a * 255)};
    }

    /// @brief Convert from an SDL_Color to a Color.
    /// @param sdlColor The SDL_Color to convert.
    /// @return The equivalent Color.
    static Color fromSDLColor(const SDL_Color& sdlColor) {
      return {sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a};
    }

    /// @brief From a hex string (e.g. "#RRGGBBAA") to a Color.
    ///
    /// @param hex The hex string to convert.
    /// @return The equivalent Color.
    ///
    static Color fromHex(const std::string& hex) {
      if (hex.empty() || hex[0] != '#' ||
          (hex.length() != 7 && hex.length() != 9)) {
        return Color();  // Invalid format, return default color
      }

      uint8_t r = std::stoi(hex.substr(1, 2), nullptr, 16);
      uint8_t g = std::stoi(hex.substr(3, 2), nullptr, 16);
      uint8_t b = std::stoi(hex.substr(5, 2), nullptr, 16);
      uint8_t a = 255;  // Default alpha

      if (hex.length() == 9) {
        a = std::stoi(hex.substr(7, 2), nullptr, 16);
      }

      return {r, g, b, a};
    }

    /// @brief Lighten a color by a given percentage.
    ///
    /// @param color The color to lighten.
    /// @param percentage The percentage to lighten (0.0-1.0).
    /// @return The lightened color.
    static Color lighten(const Color& color, float percentage) {
      return {static_cast<uint8_t>(color.r + (255 - color.r) * percentage),
              static_cast<uint8_t>(color.g + (255 - color.g) * percentage),
              static_cast<uint8_t>(color.b + (255 - color.b) * percentage),
              color.a};
    }

    /// @brief Darken a color by a given percentage.
    ///
    /// @param color The color to darken.
    /// @param percentage The percentage to darken (0.0-1.0).
    /// @return The darkened color.
    static Color darken(const Color& color, float percentage) {
      return {static_cast<uint8_t>(color.r * (1 - percentage)),
              static_cast<uint8_t>(color.g * (1 - percentage)),
              static_cast<uint8_t>(color.b * (1 - percentage)), color.a};
    }

    /// @brief Blend two colors together using alpha blending.
    ///
    /// @param foreground The foreground color (with alpha).
    /// @param background The background color.
    /// @return The blended color.
    static Color blend(const Color& foreground, const Color& background) {
      float alpha = foreground.a / 255.0F;
      uint8_t r = static_cast<uint8_t>(foreground.r * alpha +
                                       background.r * (1 - alpha));
      uint8_t g = static_cast<uint8_t>(foreground.g * alpha +
                                       background.g * (1 - alpha));
      uint8_t b = static_cast<uint8_t>(foreground.b * alpha +
                                       background.b * (1 - alpha));
      return {r, g, b, 255};
    }

    /// @brief Invert a color.
    ///
    /// @param color The color to invert.
    /// @return The inverted color.
    static Color invert(const Color& color) {
      return {static_cast<uint8_t>(255 - color.r),
              static_cast<uint8_t>(255 - color.g),
              static_cast<uint8_t>(255 - color.b), color.a};
    }

    /// @brief Convert a color to grayscale.
    ///
    /// @param color The color to convert.
    /// @return The grayscale color.
    static Color grayscale(const Color& color) {
      uint8_t gray = static_cast<uint8_t>(0.299F * color.r + 0.587F * color.g +
                                          0.114F * color.b);
      return {gray, gray, gray, color.a};
    }

    /// @brief Adjust the alpha of a color.
    ///
    /// @param color The color to adjust.
    /// @param alpha The new alpha value (0-255).
    /// @return The color with the adjusted alpha.
    static Color withAlpha(const Color& color, uint8_t alpha) {
      return {color.r, color.g, color.b, alpha};
    }
  };

  /// Color constants
  namespace color {
    // Basic color names
    inline const Color black = Color::fromHex("#000000");
    inline const Color silver = Color::fromHex("#c0c0c0");
    inline const Color gray = Color::fromHex("#808080");
    inline const Color white = Color::fromHex("#ffffff");
    inline const Color maroon = Color::fromHex("#800000");
    inline const Color red = Color::fromHex("#ff0000");
    inline const Color purple = Color::fromHex("#800080");
    inline const Color fuchsia = Color::fromHex("#ff00ff");
    inline const Color green = Color::fromHex("#008000");
    inline const Color lime = Color::fromHex("#00ff00");
    inline const Color olive = Color::fromHex("#808000");
    inline const Color yellow = Color::fromHex("#ffff00");
    inline const Color navy = Color::fromHex("#000080");
    inline const Color blue = Color::fromHex("#0000ff");
    inline const Color teal = Color::fromHex("#008080");
    inline const Color aqua = Color::fromHex("#00ffff");

    // Extended color names
    inline const Color aliceblue = Color::fromHex("#f0f8ff");
    inline const Color antiquewhite = Color::fromHex("#faebd7");
    inline const Color aquamarine = Color::fromHex("#7fffd4");
    inline const Color azure = Color::fromHex("#f0ffff");
    inline const Color beige = Color::fromHex("#f5f5dc");
    inline const Color bisque = Color::fromHex("#ffe4c4");
    inline const Color blanchedalmond = Color::fromHex("#ffebcd");
    inline const Color blueviolet = Color::fromHex("#8a2be2");
    inline const Color brown = Color::fromHex("#a52a2a");
    inline const Color burlywood = Color::fromHex("#deb887");
    inline const Color cadetblue = Color::fromHex("#5f9ea0");
    inline const Color chartreuse = Color::fromHex("#7fff00");
    inline const Color chocolate = Color::fromHex("#d2691e");
    inline const Color coral = Color::fromHex("#ff7f50");
    inline const Color cornflowerblue = Color::fromHex("#6495ed");
    inline const Color cornsilk = Color::fromHex("#fff8dc");
    inline const Color crimson = Color::fromHex("#dc143c");
    inline const Color cyan = Color::fromHex("#00ffff");
    inline const Color darkblue = Color::fromHex("#00008b");
    inline const Color darkcyan = Color::fromHex("#008b8b");
    inline const Color darkgoldenrod = Color::fromHex("#b8860b");
    inline const Color darkgray = Color::fromHex("#a9a9a9");
    inline const Color darkgreen = Color::fromHex("#006400");
    inline const Color darkgrey = Color::fromHex("#a9a9a9");
    inline const Color darkkhaki = Color::fromHex("#bdb76b");
    inline const Color darkmagenta = Color::fromHex("#8b008b");
    inline const Color darkolivegreen = Color::fromHex("#556b2f");
    inline const Color darkorange = Color::fromHex("#ff8c00");
    inline const Color darkorchid = Color::fromHex("#9932cc");
    inline const Color darkred = Color::fromHex("#8b0000");
    inline const Color darksalmon = Color::fromHex("#e9967a");
    inline const Color darkseagreen = Color::fromHex("#8fbc8f");
    inline const Color darkslateblue = Color::fromHex("#483d8b");
    inline const Color darkslategray = Color::fromHex("#2f4f4f");
    inline const Color darkslategrey = Color::fromHex("#2f4f4f");
    inline const Color darkturquoise = Color::fromHex("#00ced1");
    inline const Color darkviolet = Color::fromHex("#9400d3");
    inline const Color deeppink = Color::fromHex("#ff1493");
    inline const Color deepskyblue = Color::fromHex("#00bfff");
    inline const Color dimgray = Color::fromHex("#696969");
    inline const Color dimgrey = Color::fromHex("#696969");
    inline const Color dodgerblue = Color::fromHex("#1e90ff");
    inline const Color firebrick = Color::fromHex("#b22222");
    inline const Color floralwhite = Color::fromHex("#fffaf0");
    inline const Color forestgreen = Color::fromHex("#228b22");
    inline const Color gainsboro = Color::fromHex("#dcdcdc");
    inline const Color ghostwhite = Color::fromHex("#f8f8ff");
    inline const Color gold = Color::fromHex("#ffd700");
    inline const Color goldenrod = Color::fromHex("#daa520");
    inline const Color greenyellow = Color::fromHex("#adff2f");
    inline const Color grey = Color::fromHex("#808080");
    inline const Color honeydew = Color::fromHex("#f0fff0");
    inline const Color hotpink = Color::fromHex("#ff69b4");
    inline const Color indianred = Color::fromHex("#cd5c5c");
    inline const Color indigo = Color::fromHex("#4b0082");
    inline const Color ivory = Color::fromHex("#fffff0");
    inline const Color khaki = Color::fromHex("#f0e68c");
    inline const Color lavender = Color::fromHex("#e6e6fa");
    inline const Color lavenderblush = Color::fromHex("#fff0f5");
    inline const Color lawngreen = Color::fromHex("#7cfc00");
    inline const Color lemonchiffon = Color::fromHex("#fffacd");
    inline const Color lightblue = Color::fromHex("#add8e6");
    inline const Color lightcoral = Color::fromHex("#f08080");
    inline const Color lightcyan = Color::fromHex("#e0ffff");
    inline const Color lightgoldenrodyellow = Color::fromHex("#fafad2");
    inline const Color lightgray = Color::fromHex("#d3d3d3");
    inline const Color lightgreen = Color::fromHex("#90ee90");
    inline const Color lightgrey = Color::fromHex("#d3d3d3");
    inline const Color lightpink = Color::fromHex("#ffb6c1");
    inline const Color lightsalmon = Color::fromHex("#ffa07a");
    inline const Color lightseagreen = Color::fromHex("#20b2aa");
    inline const Color lightskyblue = Color::fromHex("#87cefa");
    inline const Color lightslategray = Color::fromHex("#778899");
    inline const Color lightslategrey = Color::fromHex("#778899");
    inline const Color lightsteelblue = Color::fromHex("#b0c4de");
    inline const Color lightyellow = Color::fromHex("#ffffe0");
    inline const Color limegreen = Color::fromHex("#32cd32");
    inline const Color linen = Color::fromHex("#faf0e6");
    inline const Color magenta = Color::fromHex("#ff00ff");
    inline const Color mediumaquamarine = Color::fromHex("#66cdaa");
    inline const Color mediumblue = Color::fromHex("#0000cd");
    inline const Color mediumorchid = Color::fromHex("#ba55d3");
    inline const Color mediumpurple = Color::fromHex("#9370db");
    inline const Color mediumseagreen = Color::fromHex("#3cb371");
    inline const Color mediumslateblue = Color::fromHex("#7b68ee");
    inline const Color mediumspringgreen = Color::fromHex("#00fa9a");
    inline const Color mediumturquoise = Color::fromHex("#48d1cc");
    inline const Color mediumvioletred = Color::fromHex("#c71585");
    inline const Color midnightblue = Color::fromHex("#191970");
    inline const Color mintcream = Color::fromHex("#f5fffa");
    inline const Color mistyrose = Color::fromHex("#ffe4e1");
    inline const Color moccasin = Color::fromHex("#ffe4b5");
    inline const Color navajowhite = Color::fromHex("#ffdead");
    inline const Color oldlace = Color::fromHex("#fdf5e6");
    inline const Color olivedrab = Color::fromHex("#6b8e23");
    inline const Color orange = Color::fromHex("#ffa500");
    inline const Color orangered = Color::fromHex("#ff4500");
    inline const Color orchid = Color::fromHex("#da70d6");
    inline const Color palegoldenrod = Color::fromHex("#eee8aa");
    inline const Color palegreen = Color::fromHex("#98fb98");
    inline const Color paleturquoise = Color::fromHex("#afeeee");
    inline const Color palevioletred = Color::fromHex("#db7093");
    inline const Color papayawhip = Color::fromHex("#ffefd5");
    inline const Color peachpuff = Color::fromHex("#ffdab9");
    inline const Color peru = Color::fromHex("#cd853f");
    inline const Color pink = Color::fromHex("#ffc0cb");
    inline const Color plum = Color::fromHex("#dda0dd");
    inline const Color powderblue = Color::fromHex("#b0e0e6");
    inline const Color rebeccapurple = Color::fromHex("#663399");
    inline const Color rosybrown = Color::fromHex("#bc8f8f");
    inline const Color royalblue = Color::fromHex("#4169e1");
    inline const Color saddlebrown = Color::fromHex("#8b4513");
    inline const Color salmon = Color::fromHex("#fa8072");
    inline const Color sandybrown = Color::fromHex("#f4a460");
    inline const Color seagreen = Color::fromHex("#2e8b57");
    inline const Color seashell = Color::fromHex("#fff5ee");
    inline const Color sienna = Color::fromHex("#a0522d");
    inline const Color skyblue = Color::fromHex("#87ceeb");
    inline const Color slateblue = Color::fromHex("#6a5acd");
    inline const Color slategray = Color::fromHex("#708090");
    inline const Color slategrey = Color::fromHex("#708090");
    inline const Color snow = Color::fromHex("#fffafa");
    inline const Color springgreen = Color::fromHex("#00ff7f");
    inline const Color steelblue = Color::fromHex("#4682b4");
    inline const Color tan = Color::fromHex("#d2b48c");
    inline const Color thistle = Color::fromHex("#d8bfd8");
    inline const Color tomato = Color::fromHex("#ff6347");
    inline const Color turquoise = Color::fromHex("#40e0d0");
    inline const Color violet = Color::fromHex("#ee82ee");
    inline const Color wheat = Color::fromHex("#f5deb3");
    inline const Color whitesmoke = Color::fromHex("#f5f5f5");
    inline const Color yellowgreen = Color::fromHex("#9acd32");

    // Special
    inline const Color transparent = Color::fromHex("#00000000");
  }  // namespace color

}  // namespace asw

#endif  // ASW_COLOR_H