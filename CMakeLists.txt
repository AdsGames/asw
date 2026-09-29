cmake_minimum_required(VERSION 3.22)

project(asw VERSION 0.3.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Only pick output folders when building asw on its own. As a dependency the
# source folder can be a cache shared by every project (CPM), so writing there
# lets separate builds overwrite each other's libraries.
if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build Release when no build type is given, unless a multi config generator
# picks it per build
set(DEFAULT_BUILD_TYPE "Release")
if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Build type" FORCE)
endif()

# asw_add_web_target() for web builds of games
include(web/AswWeb.cmake)

# Install rules for a find_package(asw) package, off by default
option(ASW_INSTALL "Install asw and the SDL libraries it links" OFF)

# Add sources
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Create lib
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Add compile options
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# SDL Flags
set(BUILD_SHARED_LIBS OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)

# Fetch deps. They stay out of the default build and install, as with CPM's
# short form, unless asw installs them for its package: the installed asw
# package needs freetype and the SDL packages installed next to it.
if(ASW_INSTALL)
  set(ASW_DEPS_EXCLUDE_FROM_ALL NO)
else()
  set(ASW_DEPS_EXCLUDE_FROM_ALL YES)
endif()

# Release archives, not git clones: a clone brings the whole history and the
# vendored libraries in each submodule, which asw does not build. That is
# about 1.9 GB against about 30 MB.
include(cmake/get_cpm.cmake)
CPMAddPackage(NAME freetype VERSION 2.14.1
  URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-14-1.tar.gz
  URL_HASH SHA256=44bd69d1f0750603410cfd4f26f1c5523c5a3a087a2dfa8639b757dc7c6677be
  EXCLUDE_FROM_ALL ${ASW_DEPS_EXCLUDE_FROM_ALL} SYSTEM YES)

if(TARGET freetype AND NOT TARGET Freetype::Freetype)
  add_library(Freetype::Freetype ALIAS freetype)
endif()

# SDL_ttf exports its targets into the build tree when it installs, which
# needs freetype exported there too
if(ASW_INSTALL AND TARGET freetype)
  export(EXPORT freetype-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/freetype-targets.cmake)
endif()

CPMAddPackage(NAME SDL VERSION 3.4.16
  URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz
  URL_HASH SHA256=7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68
  EXCLUDE_FROM_ALL ${ASW_DEPS_EXCLUDE_FROM_ALL} SYSTEM YES
  OPTIONS "SDL_INSTALL ${ASW_INSTALL}")
CPMAddPackage(NAME SDL_image VERSION 3.4.6
  URL https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.6/SDL3_image-3.4.6.tar.gz
  URL_HASH SHA256=d2e4637ae700f72e5196b8fbd749850ed2e5e1e09c5a5be8d06ff55aaccf3b01
  EXCLUDE_FROM_ALL ${ASW_DEPS_EXCLUDE_FROM_ALL} SYSTEM YES
  OPTIONS "SDLIMAGE_INSTALL ${ASW_INSTALL}")
CPMAddPackage(NAME SDL_ttf VERSION 3.2.2
  URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz
  URL_HASH SHA256=63547d58d0185c833213885b635a2c0548201cc8f301e6587c0be1a67e1e045d
  EXCLUDE_FROM_ALL ${ASW_DEPS_EXCLUDE_FROM_ALL} SYSTEM YES
  OPTIONS "SDLTTF_INSTALL ${ASW_INSTALL}")
CPMAddPackage(NAME SDL_mixer VERSION 3.2.4
  URL https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.4/SDL3_mixer-3.2.4.tar.gz
  URL_HASH SHA256=182a07c745375e113dc740d43964ff21b0be29f29f59876c4dbc4db3d32f6901
  EXCLUDE_FROM_ALL ${ASW_DEPS_EXCLUDE_FROM_ALL} SYSTEM YES
  OPTIONS "SDLMIXER_INSTALL ${ASW_INSTALL}")

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
target_link_libraries(
  ${PROJECT_NAME}
  SDL3::SDL3-static
  SDL3_image::SDL3_image-static
  SDL3_mixer::SDL3_mixer-static
  SDL3_ttf::SDL3_ttf-static
  z
)

# Examples
option(ASW_BUILD_EXAMPLES "Build ASW examples" OFF)
if(ASW_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Install
if(ASW_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(FILES web/AswWeb.cmake web/asw-web.js web/shell.html
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

  # freetype's installed config does not find the libraries it links, so the
  # asw config finds the ones freetype was built with
  set(ASW_FREETYPE_DEPENDENCIES "")
  if(TARGET freetype)
    get_target_property(freetype_libs freetype LINK_LIBRARIES)
    foreach(dep IN ITEMS ZLIB BZip2 PNG)
      if("${dep}::${dep}" IN_LIST freetype_libs)
        list(APPEND ASW_FREETYPE_DEPENDENCIES ${dep})
      endif()
    endforeach()
  endif()

  install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

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
endif()
