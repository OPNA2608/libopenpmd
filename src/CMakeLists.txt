project(p86-test)

cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror -pedantic-errors)
endif()

set(P86_SOURCES main.c p86.c)

add_executable(p86_test ${P86_SOURCES} ${P86_HEADERS})
