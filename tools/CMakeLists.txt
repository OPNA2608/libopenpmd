macro (addTool name)
	add_executable (${name} ${name}.c)
	target_link_libraries (${name} PRIVATE openpmd)
	install (TARGETS ${name}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endmacro ()

addTool (p86test)
addTool (p86exp)
addTool (p86imp)
