add_executable(example_controller main.cpp)
target_link_libraries(example_controller PRIVATE asw::asw)
asw_example_assets(example_controller)
