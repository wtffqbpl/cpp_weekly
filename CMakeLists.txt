cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_weekly)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_FLAGS "-std=c++2a")
enable_testing()

file(GLOB_RECURSE cpp_weekly_srcs *.cc)
# add_executable(cpp_weekly ${cpp_weekly_srcs})
add_executable(cpp_weekly
        conversion_function/conversion_function.cc
        class/class_test.cc
        formatting/formatting_test.cc
        functor/function_test.cc
        functional/invoke_test.cc
        future/future_test.cc
        io/io_test.cc
        pointer_like_classes/iterator_case_test.cc
        pointer_like_classes/shared_ptr_test.cc
        stl/valarray_test.cc
        strings/string_test.cc
        strings/string_view_test.cc
        template/chap_1_function_templates.cc
        template/class_template_test.cc
        template/function_template_test.cc
        )

include_directories(include)

target_link_libraries(
        cpp_weekly
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpp_weekly)