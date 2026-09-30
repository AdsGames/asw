add_executable(example_overlays main.cpp)
target_link_libraries(example_overlays PRIVATE asw::asw)
asw_example_assets(example_overlays)
