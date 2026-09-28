add_executable(example_ui main.cpp)
target_link_libraries(example_ui PRIVATE asw::asw)
asw_example_assets(example_ui)
