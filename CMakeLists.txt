cmake_minimum_required(VERSION 3.20)

project(bouncer)

enable_testing()

# if you're on linux.... good luck
list(APPEND CMAKE_MODULE_PATH "C:/Program Files (x86)/Catch2/lib/cmake/Catch2/")
find_package(Catch2 3 REQUIRED)

# Compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wextra -O0")

# Source sets
file(GLOB renderer_src CONFIGURE_DEPENDS
    "src/render/*.cpp"
)

file(GLOB test_src CONFIGURE_DEPENDS
    "src/test/*.cpp"
    "src/test/type/*.cpp"
)

include_directories("inc")

# Compile settings
add_executable(bouncer ${renderer_src})
set_target_properties(bouncer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

add_executable(tests ${test_src} )
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)


# Catch tests
include(CTest)
include(Catch)
catch_discover_tests(tests)