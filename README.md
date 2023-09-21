# asw

A.D.S. Games SDL Wrapper.

This project started as a way to easily port allegro4 games to SDL2. Now it intends to make it easier to use SDL2 in C++. A primary focus is to allow easy compilation of games using emscripten as well, and has been throughly tested with it.

## Usage (CMAKE)

### Fetch Content

```sh
FetchContent_Declare(
  asw
  GIT_REPOSITORY https://github.com/adsgames/asw.git
  GIT_TAG        <tag from latest version>
)
FetchContent_MakeAvailable(asw)
```
