cmake_minimum_required(VERSION 3.24)
project(asw VERSION 0.3.5 LANGUAGES CXX)
include(FetchContent)

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

# SDL Flags
set(BUILD_SHARED_LIBS OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

# Fetch SDL3
message(STATUS "Fetching SDL3")

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.8
    OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG release-3.2.4
    OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer
    GIT_TAG origin/main
    OVERRIDE_FIND_PACKAGE
)  

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
    GIT_TAG release-3.2.0
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(SDL3_image)
FetchContent_MakeAvailable(SDL3_mixer)
FetchContent_MakeAvailable(SDL3_ttf)

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

# Link Libs
if(MINGW)
  target_link_libraries(${PROJECT_NAME} -lmingw32)
endif(MINGW)

target_link_libraries(
  ${PROJECT_NAME}
  SDL3::SDL3-static
  SDL3_image::SDL3_image-static
  SDL3_mixer::SDL3_mixer-static
  SDL3_ttf::SDL3_ttf-static
)

if(DEFINED EMSCRIPTEN) 
  target_link_libraries(
    ${PROJECT_NAME}
    z # Zlib for SDL3_ttf
  )
endif()

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
