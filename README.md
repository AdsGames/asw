# asw

A.D.S. Games SDL Wrapper

## Usage (CMAKE)

### Add submodule

```
mkdir lib
git submodule add https://github.com/AdsGames/asw lib/asw
```

### Add to CMakeLists.txt

```
set(ASW_HEADERS lib/asw)

target_include_directories(
  executable_name PRIVATE
  ${ASW_HEADERS}
)
```
