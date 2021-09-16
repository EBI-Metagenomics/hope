cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  hope
  LANGUAGES C
  VERSION 0.0.12)
set(PROJECT_DESCRIPTION "Testing library for the C language.")

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # /wd5105: https://is.gd/d407jh
  set(WARNING_FLAGS /W3 /wd5105)
else()
  set(WARNING_FLAGS
      -Wall
      -Wextra
      -Wstrict-prototypes
      -Wshadow
      -Wconversion
      -Wmissing-prototypes
      -Wno-unused-parameter
      -Wsign-conversion
      -Wno-unused-function)

  if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND WARNING_FLAGS -Wno-gnu-designator -Wno-empty-translation-unit
         -Wno-gnu-statement-expression -Wno-nullability-extension)
  endif()
endif()

option(CI_BUILD "CI, extra flags will be set" OFF)
if(CI_BUILD)
  message(STATUS "CI build enabled")
  if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/WX)
  else()
    add_compile_options(-Werror)
  endif()
endif()

add_library(hope INTERFACE)
add_library(HOPE::hope ALIAS hope)
target_compile_features(hope INTERFACE c_std_11)
target_include_directories(
  hope INTERFACE $<INSTALL_INTERFACE:include>
                 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(hope INTERFACE $<$<BOOL:${UNIX}>:m>)

install(TARGETS hope EXPORT hope-targets)
install(DIRECTORY include/ DESTINATION include)

# Export the targets to a script
install(
  EXPORT hope-targets
  FILE hope-targets.cmake
  NAMESPACE HOPE::
  DESTINATION lib/cmake/hope)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  hope-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/hope-config.cmake
  INSTALL_DESTINATION lib/cmake/hope
  NO_SET_AND_CHECK_MACRO)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hope-config.cmake
        DESTINATION lib/cmake/hope)

enable_testing()
add_subdirectory(test)
