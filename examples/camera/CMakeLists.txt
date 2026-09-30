add_executable(example_camera main.cpp)
target_link_libraries(example_camera PRIVATE asw::asw)
asw_example_assets(example_camera)
