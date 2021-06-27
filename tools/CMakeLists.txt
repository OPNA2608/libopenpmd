project(tools)

add_executable (loadtest loadtest.c)
target_link_libraries (loadtest PRIVATE openpmd)

add_executable (p86create p86create.c)
target_link_libraries (p86create PRIVATE openpmd)
