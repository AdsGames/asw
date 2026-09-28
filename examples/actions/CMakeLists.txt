add_executable(example_actions main.cpp)
target_link_libraries(example_actions PRIVATE asw::asw)
asw_example_assets(example_actions)
