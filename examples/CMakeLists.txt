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
