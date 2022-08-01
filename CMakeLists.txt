cmake_minimum_required(VERSION 3.20)

project(cpp_weekly)

add_custom_target(test)

set(CMAKE_CXX_FLAGS "-std=c++17")

macro(add_test_target _src_list _prefix)
  foreach(_afile ${_src_list})
    get_filename_component(_target ${_afile} NAME_WE)
    add_custom_target(${_prefix}-${_target}-test
      COMMAND ${CMAKE_CXX_COMPILER} -O2 ${CMAKE_CURRENT_SOURCE_DIR}/${_afile} -o ${_target} -std=c++17
      COMMAND ./${_target}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(test ${_prefix}-${_target}-test)
  endforeach()
endmacro()

add_subdirectory(conversion_function)
add_subdirectory(ep332_lambda_vs_std_function_vs_function_pointer)
add_subdirectory(formatting)
add_subdirectory(functor)
add_subdirectory(io)
add_subdirectory(misc)
add_subdirectory(pointer_like_classes)
add_subdirectory(stl)
add_subdirectory(strings)
add_subdirectory(template)

