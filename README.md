# asw

[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_asw&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=AdsGames_asw)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_asw&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=AdsGames_asw)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=AdsGames_asw&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=AdsGames_asw)

A.D.S. Games SDL Wrapper.

This project started as a way to easily port allegro5 games to SDL3. Now it intends to make it easier to use SDL3 in C++. A primary focus is to allow easy compilation of games using emscripten as well, and has been throughly tested with it.

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

### Web Builds

`asw_add_web_target()` makes a game's Emscripten build a web page. It does nothing on other platforms, so you can call it for every build:

```cmake
asw_add_web_target(${PROJECT_NAME}
  TITLE "My Game"
  ASSETS ${CMAKE_CURRENT_LIST_DIR}/assets
)
```

The page is `index.html`. It fills the window, shows the loading progress, and stops the arrow keys, <kbd>Space</kbd> and <kbd>Tab</kbd> from scrolling the page. Options:

| Option | Default | Purpose |
| --- | --- | --- |
| `TITLE` | the target name | Page title |
| `BACKGROUND` | `#0a0a0a` | Page colour |
| `OUTPUT_NAME` | `index` | Page file name, without `.html` |
| `ASSETS` | none | Folders to preload at `/assets` |
| `SHELL` | the ASW page | Your own HTML shell. It must contain `{{{ SCRIPT }}}`, and can have a `#canvas` and a `#status` |
| `PRE_JS` | none | More `--pre-js` files |

When the game is in an iframe, it sends its loading state to the parent page with `postMessage`. The page's own status label then stays hidden, so the parent page can show the loading state:

```js
window.addEventListener("message", (event) => {
  if (event.data?.type === "asw:status") {
    // event.data.text is the Emscripten status, "" when loading is done
  } else if (event.data?.type === "asw:ready") {
    // The game started
  }
});
```

The page adds `setStatus`, `onRuntimeInitialized` and `onStop` hooks to `Module`. A custom shell can set its own hooks on `Module`, and they run after the ASW ones.

## Developing

### Building

```sh
cmake --preset debug
cmake --build --preset debug
```

Output is in the `build/debug/lib/` directory.

### Building Examples

```sh
cmake --preset debug -DASW_BUILD_EXAMPLES=ON
cmake --build --preset debug
```

Output is in the `build/debug/bin/` directory.

### Building Examples for the Browser

With [Emscripten](https://emscripten.org) installed:

```sh
emcmake cmake -S . -B build/web -DCMAKE_BUILD_TYPE=Release -DASW_BUILD_EXAMPLES=ON
cmake --build build/web
cd build/web/bin && python3 -m http.server
```

Then open `http://localhost:8000/example_particles.html`. Add `?autorun` to the address to play the example's scripted run.

Each release has an `asw-examples.zip` with every example built for the browser and a screenshot of each.
