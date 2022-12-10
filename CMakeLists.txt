cmake_minimum_required (VERSION 3.0)
project (libopenpmd C)

if (${CMAKE_VERSION} VERSION_GREATER "3.0.2")
	cmake_policy (SET CMP0054 NEW)
endif ()

set (CMAKE_C_STANDARD 90)
set (CMAKE_C_STANDARD_REQUIRED TRUE)
set (CMAKE_C_EXTENSIONS OFF)

function(mkCompilerOptions targetList)
	if (${CMAKE_C_COMPILER_ID} MATCHES "MSVC|\\*Watcom")
		set (switchChar "/")
	else ()
		set (switchChar "-")
	endif ()

	foreach (option ${ARGN})
		list (APPEND ${targetList} "${switchChar}${option}")
	endforeach ()

	set (${targetList} ${${targetList}} PARENT_SCOPE)
endfunction()

set (OPENPMD_DEFINES)
set (OPENPMD_CFLAGS)

if (MSVC)
	# warning level 4 and all warnings as errors
	# add_compile_options (/W4 /WX)
	# Shut up MSVC about its "better", non-portable functions
	# add_definitions (D_CRT_SECURE_NO_WARNINGS)
	mkCompilerOptions (OPENPMD_DEFINES D_CRT_SECURE_NO_WARNINGS)
elseif (${CMAKE_C_COMPILER_ID} MATCHES "GNU|\\.*Clang|\\.*LLVM")
	# lots of warnings and all warnings as errors
	# add_compile_options (
	mkCompilerOptions (OPENPMD_CFLAGS
		Wall
		Wextra
		Wstrict-prototypes
		Wmissing-prototypes
		Wconversion
		pedantic
		Werror
		pedantic-errors #prolly redundant
	)
endif ()

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Debug)
endif ()
string (TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)

if (CMAKE_BUILD_TYPE_UPPER MATCHES "DEBUG|RELWITHDEBINFO")
	mkCompilerOptions (OPENPMD_DEFINES DDEBUG)
endif ()

if (NOT WIN32 AND NOT CMAKE_SYSTEM_NAME STREQUAL "DOS")
	mkCompilerOptions (OPENPMD_DEFINES DENABLE_TRAP)
endif ()

#if (USE_LOWMEM)
#  add_definitions (-DUSE_LOWMEM)
#endif ()

include (GNUInstallDirs)

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (tools)
