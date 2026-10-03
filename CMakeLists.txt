cmake_minimum_required(VERSION 3.16)
project(cpp_graphics_basics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 只在 GCC/Clang 下加这些警告参数（MSVC 不认，会报错）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
endif()

# examples/ 和 mycode/ 下的每个 .cpp 自动生成一个可执行文件。
# 你往 mycode/ 里丢新的 .cpp，重新 configure 一下就会自动多出一个 target。
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")
file(GLOB MY_SOURCES      CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/mycode/*.cpp")

foreach(src IN LISTS EXAMPLE_SOURCES MY_SOURCES)
    get_filename_component(name "${src}" NAME_WE)
    add_executable(${name} "${src}")
endforeach()

# 多文件编译示范（.h + .cpp + main.cpp）
add_executable(multifile_demo examples/multifile/main.cpp examples/multifile/vec3.cpp)
target_include_directories(multifile_demo PRIVATE examples/multifile)
