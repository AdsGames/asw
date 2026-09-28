add_executable(example_mouse main.cpp)
target_link_libraries(example_mouse PRIVATE asw::asw)
asw_example_assets(example_mouse)
