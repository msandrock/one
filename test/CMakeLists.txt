include_directories(${ONE_SOURCE_DIR}/src)

add_executable(one_test test.cpp)

target_link_libraries (one_test
                       onedep
                       )

add_test (NAME OneTest COMMAND one_test)