project(tools)

add_executable (loadtest loadtest.c)
target_link_libraries (loadtest PRIVATE openpmd)

add_executable (p86create p86create.c)
target_link_libraries (p86create PRIVATE openpmd)

add_executable (p86extract p86extract.c)
target_link_libraries (p86extract PRIVATE openpmd)
