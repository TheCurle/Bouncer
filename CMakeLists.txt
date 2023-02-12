cmake_minimum_required(VERSION 3.20)
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.1.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

project(bouncer)

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
find_package(OpenMP REQUIRED)
find_package(OpenGL REQUIRED)

# Compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -O0 -fuse-ld=lld")

# Source sets
file(GLOB renderer_src
        "src/render/Main.cpp"
        "src/render/lighting/shading.cpp"
        "src/render/raycasting/intersection.cpp"
        "src/render/raycasting/patterns.cpp"
        "src/math/matrix.cpp"
)

file(GLOB test_src
        "src/test/render/TestHit.cpp"
        "src/test/render/TestIntersections.cpp"
        "src/test/render/TestLight.cpp"
        "src/test/render/TestLighting.cpp"
        "src/test/render/TestMaterial.cpp"
        "src/test/render/TestPatterns.cpp"
        "src/test/render/TestRay.cpp"
        "src/test/render/TestRefraction.cpp"
        "src/test/geometry/TestNormal.cpp"
        "src/test/geometry/TestPlane.cpp"
        "src/test/type/TestMatrix.cpp"
        "src/test/type/TestRaster.cpp"
        "src/test/type/TestTranslationRotationScale.cpp"
        "src/test/type/TestTuple.cpp"
        "src/test/view/TestCamera.cpp"
        "src/test/view/TestWorld.cpp"
)

include_directories("inc")
include_directories(${OPENGL_INCLUDE_DIR})

# Compile settings
add_executable(bouncer ${renderer_src})
set_target_properties(bouncer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
target_link_libraries(bouncer PRIVATE OpenMP::OpenMP_CXX ${OPENGL_LIBRARY} user32 gdi32 gdiplus Shlwapi dwmapi stdc++fs)

add_executable(tests ${test_src})
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain OpenMP::OpenMP_CXX)


# Catch tests
include(CTest)
include(Catch)
catch_discover_tests(tests)