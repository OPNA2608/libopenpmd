set (LIBOPENPMD_SOURCES
	common.c
	io.c
	p86.c
)

add_library (openpmd ${LIBOPENPMD_SOURCES})
target_include_directories (openpmd PUBLIC ${INCLUDE_PATH})
target_compile_definitions (openpmd PUBLIC ${OPENPMD_DEFINES})
target_compile_options (openpmd PUBLIC ${OPENPMD_CFLAGS})
install (TARGETS openpmd
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
