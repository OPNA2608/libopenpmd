add_executable (p86test p86test.c)
target_link_libraries (p86test PRIVATE openpmd)
target_include_directories (p86test PUBLIC ${INCLUDE_PATH})
install (TARGETS p86test
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_executable (p86exp p86exp.c)
target_link_libraries (p86exp PRIVATE openpmd)
target_include_directories (p86exp PUBLIC ${INCLUDE_PATH})
install (TARGETS p86exp
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
