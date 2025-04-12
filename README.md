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

## Developing

### Building

```sh
cmake -B build .
cmake --build build
```
