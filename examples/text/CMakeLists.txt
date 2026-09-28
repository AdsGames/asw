add_executable(example_text main.cpp)
target_link_libraries(example_text PRIVATE asw::asw)
asw_example_assets(example_text)
