project (p86-test)

cmake_minimum_required (VERSION 3.0)

set (CMAKE_C_STANDARD 90)
set (CMAKE_C_STANDARD_REQUIRED TRUE)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options (/W4 /WX)
else ()
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

set (P86_SOURCES main.c p86.c)

add_executable (p86_test ${P86_SOURCES} ${P86_HEADERS})
