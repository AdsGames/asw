cmake_minimum_required(VERSION 3.22)

set(ASW_EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Copy the shared example assets, and the example's own assets folder if it
# has one, next to the executable
function(asw_example_assets target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${ASW_EXAMPLES_DIR}/assets $<TARGET_FILE_DIR:${target}>/assets
  )
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${target}>/assets
    )
  endif()
endfunction()

add_subdirectory(keyboard)
add_subdirectory(mouse)
add_subdirectory(controller)
add_subdirectory(actions)
add_subdirectory(primitives)
add_subdirectory(camera)
add_subdirectory(sound)
add_subdirectory(scenes)
add_subdirectory(particles)
add_subdirectory(easing)
add_subdirectory(lighting)
add_subdirectory(text)
add_subdirectory(ui)

# Browser builds: each example becomes an .html page with its assets packed
# into a .data file, so it can be embedded in the docs and played on the web
if(EMSCRIPTEN)
  set(ASW_EXAMPLE_TARGETS
    example_keyboard example_mouse example_controller example_actions
    example_primitives example_camera example_sound example_scenes
    example_particles example_easing example_lighting example_text example_ui
  )

  foreach(target IN LISTS ASW_EXAMPLE_TARGETS)
    get_target_property(example_dir ${target} SOURCE_DIR)

    set(assets ${ASW_EXAMPLES_DIR}/assets)
    if(EXISTS ${example_dir}/assets)
      list(APPEND assets ${example_dir}/assets)
    endif()

    asw_add_web_target(${target}
      TITLE "ASW example"
      BACKGROUND "#111318"
      OUTPUT_NAME ${target}
      ASSETS ${assets}
      PRE_JS ${ASW_EXAMPLES_DIR}/web/autorun.js
    )
    target_link_options(${target} PRIVATE "-sEXPORTED_RUNTIME_METHODS=ENV")
  endforeach()
endif()
