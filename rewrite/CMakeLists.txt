cmake_minimum_required (VERSION 3.0)
project (libopenpmd C)

if (${CMAKE_VERSION} VERSION_GREATER "3.0.2")
	cmake_policy (SET CMP0054 NEW)
endif ()

set (CMAKE_C_STANDARD 90)
set (CMAKE_C_STANDARD_REQUIRED TRUE)
set (CMAKE_C_EXTENSIONS OFF)

if (MSVC)
	# warning level 4 and all warnings as errors
	# add_compile_options (/W4 /WX)
	# Shut up MSVC about its "better", non-portable functions
	add_definitions (/D_CRT_SECURE_NO_WARNINGS)
elseif (${CMAKE_C_COMPILER_ID} MATCHES "GNU|\\.*Clang|\\.*LLVM")
	# lots of warnings and all warnings as errors
	add_compile_options (-Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -Werror -pedantic-errors)
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

#if (CMAKE_SYSTEM_NAME STREQUAL "DOS" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "I86")
#	set (PREFER_LOWMEM TRUE)
#else ()
#	set (PREFER_LOWMEM FALSE)
#endif ()
#option (USE_LOWMEM
#	"Use code that's better suited for low-memory environments"
#	${PREFER_LOWMEM}
#)
#message (STATUS "USE_LOWMEM: ${USE_LOWMEM}")

#if (USE_LOWMEM)
#  add_definitions (-DUSE_LOWMEM)
#endif ()

include (GNUInstallDirs)

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (tools)
