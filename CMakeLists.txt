cmake_minimum_required(VERSION 3.20)

project(bouncer)

find_package(OpenMP REQUIRED)
find_package(OpenGL REQUIRED)

# Compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -O0 -fuse-ld=lld")

# Source sets
file(GLOB renderer_src CONFIGURE_DEPENDS
        "src/render/*.cpp"
)

include_directories("inc")
include_directories(${OPENGL_INCLUDE_DIR})

# Compile settings
add_executable(bouncer ${renderer_src})
set_target_properties(bouncer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
target_link_libraries(bouncer PRIVATE OpenMP::OpenMP_CXX ${OPENGL_LIBRARY} user32 gdi32 gdiplus Shlwapi dwmapi stdc++fs)
