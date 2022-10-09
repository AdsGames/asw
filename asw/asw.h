#ifndef ASW_H
#define ASW_H

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <string>

namespace asw {

  typedef std::shared_ptr<SDL_Texture> Texture;
  typedef std::shared_ptr<TTF_Font> Font;
  typedef std::shared_ptr<Mix_Chunk> Sample;
  typedef SDL_Renderer Renderer;
  typedef SDL_Window Window;
  typedef SDL_Color Color;

  /**
   * @brief Display
   *
   */
  namespace display {

    // Renderer
    extern asw::Renderer* renderer;
    extern asw::Window* window;

    inline void setTitle(const std::string& title) {
      SDL_SetWindowTitle(asw::display::window, title.c_str());
    }

    inline void setIcon(const std::string& path) {
      SDL_Surface* icon = IMG_Load(path.c_str());

      if (!icon) {
        return;
      }

      SDL_SetWindowIcon(asw::display::window, icon);
    }

    inline void setFullscreen(bool fullscreen) {
      if (fullscreen) {
        SDL_SetWindowFullscreen(asw::display::window, SDL_WINDOW_FULLSCREEN);
      } else {
        SDL_SetWindowFullscreen(asw::display::window, 0);
      }
    }

    inline void setResolution(int w, int h) {
      SDL_SetWindowSize(asw::display::window, w, h);
    }

    inline SDL_Point getSize() {
      SDL_Point size;
      SDL_GetWindowSize(asw::display::window, &size.x, &size.y);
      return size;
    }

    inline SDL_Point getLogicalSize() {
      SDL_Point size;
      SDL_RenderGetLogicalSize(asw::display::renderer, &size.x, &size.y);
      return size;
    }

    inline SDL_FPoint getScale() {
      SDL_FPoint scale;
      SDL_RenderGetScale(asw::display::renderer, &scale.x, &scale.y);
      return scale;
    }

  }  // namespace display

  namespace core {

    extern bool exit;

    inline void update() {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        switch (e.type) {
          case SDL_WINDOWEVENT:
            switch (e.window.event) {
              case SDL_WINDOWEVENT_RESIZED: {
                // Maintain aspect ratio
                SDL_FPoint scale;
                SDL_RenderGetScale(asw::display::renderer, &scale.x, &scale.y);

                SDL_Point size;
                SDL_RenderGetLogicalSize(asw::display::renderer, &size.x,
                                         &size.y);

                SDL_SetWindowSize(asw::display::window, size.x * scale.x,
                                  size.y * scale.y);
                break;
              }
            }
            break;

          case SDL_QUIT:
            exit = true;
            break;

          default:
            break;
        }
      }
    }
  }  // namespace core

  /**
   * @brief Utility routines
   *
   */
  namespace util {

    inline void abortOnError(const std::string& message) {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(),
                               nullptr);
      exit(-1);
    }

    inline void init(int width, int height, int scale = 1) {
      if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        abortOnError("SDL_Init");
      }

      // Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;
      if (!(IMG_Init(imgFlags) & imgFlags)) {
        abortOnError("IMG_Init");
      }

      if (TTF_Init()) {
        abortOnError("TTF_Init");
      }

      // Initialize SDL_mixer
      if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        abortOnError("Mix_OpenAudio");
      }

      asw::display::window = SDL_CreateWindow(
          "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * scale,
          height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

      if (!asw::display::window) {
        asw::util::abortOnError("WINDOW");
      }

      // Get window surface
      asw::display::renderer = SDL_CreateRenderer(asw::display::window, -1,
                                                  SDL_RENDERER_ACCELERATED);

      SDL_RenderSetLogicalSize(asw::display::renderer, width, height);

      SDL_SetRenderDrawBlendMode(asw::display::renderer, SDL_BLENDMODE_BLEND);
    }

    inline Color makeColor(int r, int g, int b) {
      Color color;
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = 255;
      return color;
    }

    inline Color makeColor(int r, int g, int b, int a) {
      Color color;
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = a;
      return color;
    }

    inline SDL_Point getTextureSize(asw::Texture tex) {
      SDL_Point size;
      SDL_QueryTexture(tex.get(), nullptr, nullptr, &size.x, &size.y);
      return size;
    }

    inline SDL_Point getTextSize(asw::Font font, const std::string& text) {
      SDL_Point size;
      TTF_SizeText(font.get(), text.c_str(), &size.x, &size.y);
      return size;
    }

  }  // namespace util

  /**
   * @brief Draw routines
   *
   */
  namespace draw {

    inline void clearColor(asw::Color color) {
      SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                             color.a);
      SDL_RenderClear(asw::display::renderer);
    }

    inline void sprite(asw::Texture tex, int x, int y) {
      SDL_Point size = asw::util::getTextureSize(tex);
      SDL_Rect dest = {x, y, size.x, size.y};
      SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
    }

    inline void spriteHFlip(asw::Texture tex, int x, int y) {
      SDL_Point size = asw::util::getTextureSize(tex);
      SDL_Rect dest = {x, y, size.x, size.y};
      SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
    }

    inline void spriteVFlip(asw::Texture tex, int x, int y) {
      SDL_Point size = asw::util::getTextureSize(tex);
      SDL_Rect dest = {x, y, size.x, size.y};
      SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
    }

    inline void spriteVHFlip(asw::Texture tex, int x, int y) {
      SDL_Point size = asw::util::getTextureSize(tex);
      SDL_Rect dest = {x, y, size.x, size.y};
      SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
    }

    inline void stretchSprite(asw::Texture tex, int x, int y, int w, int h) {
      SDL_Rect dest = {x, y, w, h};
      SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
    }

    inline void stretchSpriteBlit(asw::Texture tex,
                                  int x1,
                                  int y1,
                                  int w1,
                                  int h1,
                                  int x2,
                                  int y2,
                                  int w2,
                                  int h2) {
      const SDL_Rect src = {x1, y1, w1, h1};
      SDL_Rect dest = {x2, y2, w2, h2};
      SDL_RenderCopy(asw::display::renderer, tex.get(), &src, &dest);
    }

    inline void text(asw::Font font,
                     std::string text,
                     int x,
                     int y,
                     asw::Color color) {
      SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
      SDL_Surface* textSurface =
          TTF_RenderText_Solid(font.get(), text.c_str(), sdlColor);
      SDL_Texture* textTexture =
          SDL_CreateTextureFromSurface(asw::display::renderer, textSurface);
      SDL_Rect dest = {x, y, textSurface->w, textSurface->h};
      SDL_RenderCopy(asw::display::renderer, textTexture, nullptr, &dest);
      SDL_FreeSurface(textSurface);
      SDL_DestroyTexture(textTexture);
    }

    inline void textCenter(asw::Font font,
                           std::string text,
                           int x,
                           int y,
                           asw::Color color) {
      auto size = asw::util::getTextSize(font, text);
      asw::draw::text(font, text, x - size.x / 2, y, color);
    }

    inline void primRectFill(int x1, int y1, int x2, int y2, asw::Color color) {
      SDL_Rect rect = {x1, y1, x2 - x1, y2 - y1};
      SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                             color.a);
      SDL_RenderFillRect(asw::display::renderer, &rect);
    }

    inline void primRect(int x1, int y1, int x2, int y2, asw::Color color) {
      SDL_Rect rect = {x1, y1, x2 - x1, y2 - y1};
      SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                             color.a);
      SDL_RenderDrawRect(asw::display::renderer, &rect);
    }

  }  // namespace draw

  /**
   * @brief Load routines
   *
   */
  namespace load {

    inline asw::Texture texture(const std::string& filename) {
      SDL_Texture* temp =
          IMG_LoadTexture(asw::display::renderer, filename.c_str());

      if (!temp) {
        util::abortOnError("Failed to load texture: " + filename);
      }

      return std::shared_ptr<SDL_Texture>(temp, SDL_DestroyTexture);
    }

    inline asw::Font font(const std::string& filename, int size) {
      TTF_Font* temp = TTF_OpenFont(filename.c_str(), size);

      if (!temp) {
        util::abortOnError("Failed to load font: " + filename);
      }
      return std::shared_ptr<TTF_Font>(temp, TTF_CloseFont);
    }

    inline asw::Sample sample(const std::string& filename) {
      Mix_Chunk* temp = Mix_LoadWAV(filename.c_str());

      if (!temp) {
        util::abortOnError("Failed to load sample: " + filename);
      }
      return std::shared_ptr<Mix_Chunk>(temp, Mix_FreeChunk);
    }

    inline asw::Texture createBitmap(int w, int h) {
      SDL_Surface* temp = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
      SDL_Texture* text =
          SDL_CreateTextureFromSurface(asw::display::renderer, temp);
      SDL_FreeSurface(temp);
      return std::shared_ptr<SDL_Texture>(text, SDL_DestroyTexture);
    }
  }  // namespace load

  /**
   * @brief Sound routines
   *
   */
  namespace sound {
    inline void play(asw::Sample sample,
                     int volume = 255,
                     int pan = 128,
                     int loop = 0) {
      int channel = Mix_GroupAvailable(0);
      Mix_VolumeChunk(sample.get(), volume);
      Mix_SetPanning(channel, pan, 255 - pan);
      Mix_PlayChannel(-1, sample.get(), loop);
    }
  }  // namespace sound
}  // namespace asw

#endif  // ASW_H