cmake_minimum_required(VERSION 3.24)
project(asw VERSION 0.3.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DEFAULT_BUILD_TYPE "Release")

# Add sources
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Create lib
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Add compile options
target_compile_options(${PROJECT_NAME} PRIVATE -O2 -Wall -Wextra -pedantic)

# Add include
target_include_directories(
  ${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE src
)

# Versioning
set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1
)

# Find libs
if(NOT EMSCRIPTEN)
  find_library(SDL_LIBRARY NAMES SDL2 REQUIRED)
  find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer REQUIRED)
  find_library(SDL_IMAGE_LIBRARY NAMES SDL2_image REQUIRED)
  find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf REQUIRED)
  find_library(SDL_MAIN_LIBRARY NAMES SDL2main REQUIRED)
endif(NOT EMSCRIPTEN)

# Link Libs
# Emscripten support
if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sSDL2_IMAGE_FORMATS=["png"]
  )
  target_link_libraries(
    ${PROJECT_NAME}
    -sWASM=1
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2
    -sUSE_SDL_MIXER=2
    -sSDL2_IMAGE_FORMATS=["png"]
    -sDEMANGLE_SUPPORT=1
    -sTOTAL_MEMORY=512MB
  )
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    LINK_FLAGS
    "--preload-file ${CMAKE_CURRENT_LIST_DIR}/assets@/assets --use-preload-plugins"
  )

# Run of the mill executable
else(EMSCRIPTEN)
  if(MINGW)
    target_link_libraries(${PROJECT_NAME} -lmingw32)
  endif(MINGW)

  target_link_libraries(
    ${PROJECT_NAME}
    -lm
    ${SDL_MAIN_LIBRARY}
    ${SDL_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_IMAGE_LIBRARY}
    ${SDL_TTF_LIBRARY}
  )
endif(EMSCRIPTEN)

# Install
include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
