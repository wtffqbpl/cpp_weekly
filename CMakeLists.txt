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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-std=c++2a")
enable_testing()

file(GLOB_RECURSE cpp_weekly_srcs *.cc)
# add_executable(cpp_weekly ${cpp_weekly_srcs})
add_executable(cpp_weekly
        algorithms/accumulate_test.cc
        algorithms/partition_test.cc
        algorithms/transform_test.cc
        algorithms/std_generate_test.cc

        concurrent/create_thread.cc
        concurrent/producer_consumer.cc

        conversion_function/conversion_function.cc

        class/class_test.cc
        class/inheritance_test.cc

        formatting/formatting_test.cc

        functor/function_test.cc

        functional/functional_test.cc

        functional_programming/chap_1_introduction.cc
        functional_programming/function_object.cc

        future/future_test.cc

        io/io_test.cc

        meta_programming/limits_test.cc
        meta_programming/type_traits_test.cc

        misc/class_related_hash_function.cc
        misc/constexpr_test.cc
        misc/initializer_list_test.cc
        misc/new_delete_test.cc
        misc/optional_test.cc
        misc/random_test.cc
        misc/shared_mutex_test.cc
        misc/std_copy_test.cc
        misc/typeinfo_test.cc
        misc/reference_cnt_demo/reference_cnt.cc

        new_features/cpp_17/inline_variable/inline_variable_main.cc
        new_features/cpp_17/inline_variable/func.cc

        optimizations/small_string_optimization.cc

        pointer_like_classes/iterator_case_test.cc
        pointer_like_classes/shared_ptr_test.cc

        stl/valarray_test.cc
        stl/iterator_test.cc
        stl/tuple_test.cc
        stl/vector_test.cc

        regrex/match_search_test.cc

        strings/string_test.cc
        strings/string_view_test.cc

        template/appendix_b.cc
        template/chap_1_function_templates.cc
        template/chap_2_class_templates.cc
        template/chap_3_nontype_template_parameters.cc
        template/chap_4_variadic_templates.cc
        template/chap_5_tricky_basics.cc
        template/chap_6_move_semantics.cc
        template/chap_7_by_value_or_by_reference.cc
        template/chap_8_compile_time_programming.cc
        template/chap_11_generic_libraries.cc
        template/chap_12_fundamentals_in_depth.cc
        template/class_template_test.cc
        template/CTAD_class_template_argument_deduction.cc
        template/enable_if_test.cc
        template/function_template_test.cc
        template/partial_specialization_test.cc
        template/ref_cref_test.cc
        template/sfinae_test.cc
        template/using_template_test.cc

        topics/deep_vs_shallow/deep_vs_shallow_main.cc
        topics/memory_block_management/memory_block_management_main.cc

        youtube/cpp_weekly_youtube_test.cc
        youtube/e340_string_split_test.cc
        )

include_directories(include
        new_features/cpp_17/inline_variable
        misc/reference_cnt_demo
        topics/deep_vs_shallow
        topics/memory_block_management
        )

target_link_libraries(
        cpp_weekly
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpp_weekly)