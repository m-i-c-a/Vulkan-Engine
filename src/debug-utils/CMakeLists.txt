add_library(debug-utils STATIC 
    DebugUtilsEXT.cpp DebugUtilsEXT.hpp
)

target_compile_options(debug-utils PRIVATE -g -std=c++17)