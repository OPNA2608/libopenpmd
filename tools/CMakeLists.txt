add_executable (loadtest loadtest.c)
target_link_libraries (loadtest PRIVATE openpmd)
target_include_directories (loadtest PUBLIC ${INCLUDE_PATH})
install (TARGETS loadtest
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_executable (p86create p86create.c)
target_link_libraries (p86create PRIVATE openpmd)
target_include_directories (p86create PUBLIC ${INCLUDE_PATH})
install (TARGETS p86create
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_executable (p86extract p86extract.c)
target_link_libraries (p86extract PRIVATE openpmd)
target_include_directories (p86extract PUBLIC ${INCLUDE_PATH})
install (TARGETS p86extract
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
