add_executable(demo demo.cpp)
target_link_libraries(demo indicators::indicators)
target_include_directories(demo PUBLIC ${PROJECT_SOURCE_DIR}/include)