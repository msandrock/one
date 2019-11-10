add_library (onedep stdafx.hpp one.hpp one.cpp)
add_executable (one main.cpp)
target_link_libraries (one onedep)