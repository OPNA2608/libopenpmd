cmake_minimum_required (VERSION 3.0)
project (libopenpmd C)

set (CMAKE_C_STANDARD 90)
set (CMAKE_C_STANDARD_REQUIRED TRUE)

if (${CMAKE_C_COMPILER_ID} MATCHES "MSVC")
    # warning level 4 and all warnings as errors
    # add_compile_options (/W4 /WX)
elseif (${CMAKE_C_COMPILER_ID} MATCHES "GNU|\\.*Clang|\\.*LLVM")
    # lots of warnings and all warnings as errors
    add_compile_options (-Wall -Wextra -pedantic -Werror -pedantic-errors)
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()
string (TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)

if (CMAKE_BUILD_TYPE_UPPER STREQUAL "DEBUG")
    if (MSVC)
        add_definitions (/DDEBUG)
    else ()
        add_definitions (-DDEBUG)
    endif ()
endif ()

include_directories( include )

# Library
add_subdirectory(src)

# CLI
add_subdirectory(tools)
