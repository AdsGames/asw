# Web builds for ASW games
#
# asw_add_web_target(<target>
#   [TITLE <title>]           Page title, defaults to the target name
#   [BACKGROUND <color>]      Page colour, defaults to #0a0a0a
#   [OUTPUT_NAME <name>]      Page file name without .html, defaults to index
#   [SHELL <file>]            Own HTML shell instead of the ASW one. It needs
#                             {{{ SCRIPT }}}, and a #canvas and #status to use
#                             them.
#   [ASSETS <dir>...]         Folders to preload at /assets
#   [PRE_JS <file>...])       More --pre-js files
#
# Makes <target> a web page that shows loading progress, keeps the keys the
# game uses from scrolling the page, and reports its loading to the parent
# page when it is in an iframe (see asw-web.js). Does nothing when the build is
# not for Emscripten, so games can call it on every platform.

function(asw_add_web_target target)
  if(NOT EMSCRIPTEN)
    return()
  endif()

  # The folder of this file, also when asw is a dependency or installed
  set(web_dir ${CMAKE_CURRENT_FUNCTION_LIST_DIR})

  cmake_parse_arguments(PARSE_ARGV 1 ARG
    "" "TITLE;BACKGROUND;OUTPUT_NAME;SHELL" "ASSETS;PRE_JS")

  if(NOT ARG_TITLE)
    set(ARG_TITLE ${target})
  endif()
  if(NOT ARG_BACKGROUND)
    set(ARG_BACKGROUND "#0a0a0a")
  endif()
  if(NOT ARG_OUTPUT_NAME)
    set(ARG_OUTPUT_NAME index)
  endif()

  if(ARG_SHELL)
    set(shell ${ARG_SHELL})
  else()
    set(ASW_WEB_TITLE ${ARG_TITLE})
    set(ASW_WEB_BACKGROUND ${ARG_BACKGROUND})
    set(shell ${CMAKE_CURRENT_BINARY_DIR}/${target}-shell.html)
    configure_file(${web_dir}/shell.html ${shell} @ONLY)
  endif()

  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME ${ARG_OUTPUT_NAME}
    SUFFIX ".html"
  )

  target_link_options(${target} PRIVATE
    "--shell-file=${shell}"
    "--extern-pre-js=${web_dir}/asw-web.js"
    "-sALLOW_MEMORY_GROWTH=1"
  )
  foreach(dir IN LISTS ARG_ASSETS)
    target_link_options(${target} PRIVATE "--preload-file=${dir}@/assets")
  endforeach()
  foreach(file IN LISTS ARG_PRE_JS)
    target_link_options(${target} PRIVATE "--pre-js=${file}")
  endforeach()

  set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS
    ${shell} ${web_dir}/asw-web.js ${ARG_PRE_JS})
endfunction()
