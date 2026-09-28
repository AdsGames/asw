add_executable(example_sound main.cpp)
target_link_libraries(example_sound PRIVATE asw::asw)
asw_example_assets(example_sound)
