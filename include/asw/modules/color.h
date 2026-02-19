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

    /// @brief From float RGBA values (0.0-1.0) to Color.
    ///
    /// @param r Red channel (0.0-1.0).
    /// @param g Green channel (0.0-1.0).
    /// @param b Blue channel (0.0-1.0).
    /// @param a Alpha channel (0.0-1.0, default 1.0).
    /// @return The equivalent Color.
    ///
    static Color from_float(float r, float g, float b, float a = 1.0F) {
      return {static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255),
              static_cast<uint8_t>(b * 255), static_cast<uint8_t>(a * 255)};
    }

    /// @brief From a hex string (e.g. "#RRGGBBAA") to a Color.
    ///
    /// @param hex The hex string to convert.
    /// @return The equivalent Color.
    ///
    static Color from_hex(const std::string& hex) {
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
    /// @param percentage The percentage to lighten (0.0-1.0).
    /// @return The lightened color.
    Color lighten(float percentage) const {
      return {static_cast<uint8_t>(r + (255 - r) * percentage),
              static_cast<uint8_t>(g + (255 - g) * percentage),
              static_cast<uint8_t>(b + (255 - b) * percentage), a};
    }

    /// @brief Darken a color by a given percentage.
    ///
    /// @param percentage The percentage to darken (0.0-1.0).
    /// @return The darkened color.
    Color darken(float percentage) {
      return {static_cast<uint8_t>(r * (1 - percentage)),
              static_cast<uint8_t>(g * (1 - percentage)),
              static_cast<uint8_t>(b * (1 - percentage)), a};
    }

    /// @brief Blend two colors together using alpha blending.
    ///
    /// @param color The color to blend with.
    /// @return The blended color.
    Color blend(const Color& color) {
      float alpha = a / 255.0F;
      return {static_cast<uint8_t>(r * alpha + color.r * (1 - alpha)),
              static_cast<uint8_t>(g * alpha + color.g * (1 - alpha)),
              static_cast<uint8_t>(b * alpha + color.b * (1 - alpha)), 255};
    }

    /// @brief Invert a color.
    ///
    /// @return The inverted color.
    Color invert() const {
      return {static_cast<uint8_t>(255 - r), static_cast<uint8_t>(255 - g),
              static_cast<uint8_t>(255 - b), a};
    }

    /// @brief Convert a color to grayscale.
    ///
    /// @param color The color to convert.
    Color grayscale() const {
      uint8_t gray = static_cast<uint8_t>(0.299F * r + 0.587F * g + 0.114F * b);
      return {gray, gray, gray, a};
    }

    /// @brief Adjust the alpha of a color.
    ///
    /// @param color The color to adjust.
    /// @param alpha The new alpha value (0-255).
    /// @return The color with the adjusted alpha.
    Color with_alpha(uint8_t alpha) const { return {r, g, b, alpha}; }
  };

  /// Color constants
  namespace color {
    // Basic color names
    inline const Color black = Color::from_hex("#000000");
    inline const Color silver = Color::from_hex("#c0c0c0");
    inline const Color gray = Color::from_hex("#808080");
    inline const Color white = Color::from_hex("#ffffff");
    inline const Color maroon = Color::from_hex("#800000");
    inline const Color red = Color::from_hex("#ff0000");
    inline const Color purple = Color::from_hex("#800080");
    inline const Color fuchsia = Color::from_hex("#ff00ff");
    inline const Color green = Color::from_hex("#008000");
    inline const Color lime = Color::from_hex("#00ff00");
    inline const Color olive = Color::from_hex("#808000");
    inline const Color yellow = Color::from_hex("#ffff00");
    inline const Color navy = Color::from_hex("#000080");
    inline const Color blue = Color::from_hex("#0000ff");
    inline const Color teal = Color::from_hex("#008080");
    inline const Color aqua = Color::from_hex("#00ffff");

    // Extended color names
    inline const Color aliceblue = Color::from_hex("#f0f8ff");
    inline const Color antiquewhite = Color::from_hex("#faebd7");
    inline const Color aquamarine = Color::from_hex("#7fffd4");
    inline const Color azure = Color::from_hex("#f0ffff");
    inline const Color beige = Color::from_hex("#f5f5dc");
    inline const Color bisque = Color::from_hex("#ffe4c4");
    inline const Color blanchedalmond = Color::from_hex("#ffebcd");
    inline const Color blueviolet = Color::from_hex("#8a2be2");
    inline const Color brown = Color::from_hex("#a52a2a");
    inline const Color burlywood = Color::from_hex("#deb887");
    inline const Color cadetblue = Color::from_hex("#5f9ea0");
    inline const Color chartreuse = Color::from_hex("#7fff00");
    inline const Color chocolate = Color::from_hex("#d2691e");
    inline const Color coral = Color::from_hex("#ff7f50");
    inline const Color cornflowerblue = Color::from_hex("#6495ed");
    inline const Color cornsilk = Color::from_hex("#fff8dc");
    inline const Color crimson = Color::from_hex("#dc143c");
    inline const Color cyan = Color::from_hex("#00ffff");
    inline const Color darkblue = Color::from_hex("#00008b");
    inline const Color darkcyan = Color::from_hex("#008b8b");
    inline const Color darkgoldenrod = Color::from_hex("#b8860b");
    inline const Color darkgray = Color::from_hex("#a9a9a9");
    inline const Color darkgreen = Color::from_hex("#006400");
    inline const Color darkgrey = Color::from_hex("#a9a9a9");
    inline const Color darkkhaki = Color::from_hex("#bdb76b");
    inline const Color darkmagenta = Color::from_hex("#8b008b");
    inline const Color darkolivegreen = Color::from_hex("#556b2f");
    inline const Color darkorange = Color::from_hex("#ff8c00");
    inline const Color darkorchid = Color::from_hex("#9932cc");
    inline const Color darkred = Color::from_hex("#8b0000");
    inline const Color darksalmon = Color::from_hex("#e9967a");
    inline const Color darkseagreen = Color::from_hex("#8fbc8f");
    inline const Color darkslateblue = Color::from_hex("#483d8b");
    inline const Color darkslategray = Color::from_hex("#2f4f4f");
    inline const Color darkslategrey = Color::from_hex("#2f4f4f");
    inline const Color darkturquoise = Color::from_hex("#00ced1");
    inline const Color darkviolet = Color::from_hex("#9400d3");
    inline const Color deeppink = Color::from_hex("#ff1493");
    inline const Color deepskyblue = Color::from_hex("#00bfff");
    inline const Color dimgray = Color::from_hex("#696969");
    inline const Color dimgrey = Color::from_hex("#696969");
    inline const Color dodgerblue = Color::from_hex("#1e90ff");
    inline const Color firebrick = Color::from_hex("#b22222");
    inline const Color floralwhite = Color::from_hex("#fffaf0");
    inline const Color forestgreen = Color::from_hex("#228b22");
    inline const Color gainsboro = Color::from_hex("#dcdcdc");
    inline const Color ghostwhite = Color::from_hex("#f8f8ff");
    inline const Color gold = Color::from_hex("#ffd700");
    inline const Color goldenrod = Color::from_hex("#daa520");
    inline const Color greenyellow = Color::from_hex("#adff2f");
    inline const Color grey = Color::from_hex("#808080");
    inline const Color honeydew = Color::from_hex("#f0fff0");
    inline const Color hotpink = Color::from_hex("#ff69b4");
    inline const Color indianred = Color::from_hex("#cd5c5c");
    inline const Color indigo = Color::from_hex("#4b0082");
    inline const Color ivory = Color::from_hex("#fffff0");
    inline const Color khaki = Color::from_hex("#f0e68c");
    inline const Color lavender = Color::from_hex("#e6e6fa");
    inline const Color lavenderblush = Color::from_hex("#fff0f5");
    inline const Color lawngreen = Color::from_hex("#7cfc00");
    inline const Color lemonchiffon = Color::from_hex("#fffacd");
    inline const Color lightblue = Color::from_hex("#add8e6");
    inline const Color lightcoral = Color::from_hex("#f08080");
    inline const Color lightcyan = Color::from_hex("#e0ffff");
    inline const Color lightgoldenrodyellow = Color::from_hex("#fafad2");
    inline const Color lightgray = Color::from_hex("#d3d3d3");
    inline const Color lightgreen = Color::from_hex("#90ee90");
    inline const Color lightgrey = Color::from_hex("#d3d3d3");
    inline const Color lightpink = Color::from_hex("#ffb6c1");
    inline const Color lightsalmon = Color::from_hex("#ffa07a");
    inline const Color lightseagreen = Color::from_hex("#20b2aa");
    inline const Color lightskyblue = Color::from_hex("#87cefa");
    inline const Color lightslategray = Color::from_hex("#778899");
    inline const Color lightslategrey = Color::from_hex("#778899");
    inline const Color lightsteelblue = Color::from_hex("#b0c4de");
    inline const Color lightyellow = Color::from_hex("#ffffe0");
    inline const Color limegreen = Color::from_hex("#32cd32");
    inline const Color linen = Color::from_hex("#faf0e6");
    inline const Color magenta = Color::from_hex("#ff00ff");
    inline const Color mediumaquamarine = Color::from_hex("#66cdaa");
    inline const Color mediumblue = Color::from_hex("#0000cd");
    inline const Color mediumorchid = Color::from_hex("#ba55d3");
    inline const Color mediumpurple = Color::from_hex("#9370db");
    inline const Color mediumseagreen = Color::from_hex("#3cb371");
    inline const Color mediumslateblue = Color::from_hex("#7b68ee");
    inline const Color mediumspringgreen = Color::from_hex("#00fa9a");
    inline const Color mediumturquoise = Color::from_hex("#48d1cc");
    inline const Color mediumvioletred = Color::from_hex("#c71585");
    inline const Color midnightblue = Color::from_hex("#191970");
    inline const Color mintcream = Color::from_hex("#f5fffa");
    inline const Color mistyrose = Color::from_hex("#ffe4e1");
    inline const Color moccasin = Color::from_hex("#ffe4b5");
    inline const Color navajowhite = Color::from_hex("#ffdead");
    inline const Color oldlace = Color::from_hex("#fdf5e6");
    inline const Color olivedrab = Color::from_hex("#6b8e23");
    inline const Color orange = Color::from_hex("#ffa500");
    inline const Color orangered = Color::from_hex("#ff4500");
    inline const Color orchid = Color::from_hex("#da70d6");
    inline const Color palegoldenrod = Color::from_hex("#eee8aa");
    inline const Color palegreen = Color::from_hex("#98fb98");
    inline const Color paleturquoise = Color::from_hex("#afeeee");
    inline const Color palevioletred = Color::from_hex("#db7093");
    inline const Color papayawhip = Color::from_hex("#ffefd5");
    inline const Color peachpuff = Color::from_hex("#ffdab9");
    inline const Color peru = Color::from_hex("#cd853f");
    inline const Color pink = Color::from_hex("#ffc0cb");
    inline const Color plum = Color::from_hex("#dda0dd");
    inline const Color powderblue = Color::from_hex("#b0e0e6");
    inline const Color rebeccapurple = Color::from_hex("#663399");
    inline const Color rosybrown = Color::from_hex("#bc8f8f");
    inline const Color royalblue = Color::from_hex("#4169e1");
    inline const Color saddlebrown = Color::from_hex("#8b4513");
    inline const Color salmon = Color::from_hex("#fa8072");
    inline const Color sandybrown = Color::from_hex("#f4a460");
    inline const Color seagreen = Color::from_hex("#2e8b57");
    inline const Color seashell = Color::from_hex("#fff5ee");
    inline const Color sienna = Color::from_hex("#a0522d");
    inline const Color skyblue = Color::from_hex("#87ceeb");
    inline const Color slateblue = Color::from_hex("#6a5acd");
    inline const Color slategray = Color::from_hex("#708090");
    inline const Color slategrey = Color::from_hex("#708090");
    inline const Color snow = Color::from_hex("#fffafa");
    inline const Color springgreen = Color::from_hex("#00ff7f");
    inline const Color steelblue = Color::from_hex("#4682b4");
    inline const Color tan = Color::from_hex("#d2b48c");
    inline const Color thistle = Color::from_hex("#d8bfd8");
    inline const Color tomato = Color::from_hex("#ff6347");
    inline const Color turquoise = Color::from_hex("#40e0d0");
    inline const Color violet = Color::from_hex("#ee82ee");
    inline const Color wheat = Color::from_hex("#f5deb3");
    inline const Color whitesmoke = Color::from_hex("#f5f5f5");
    inline const Color yellowgreen = Color::from_hex("#9acd32");

    // Special
    inline const Color transparent = Color::from_hex("#00000000");
  }  // namespace color

}  // namespace asw

#endif  // ASW_COLOR_H