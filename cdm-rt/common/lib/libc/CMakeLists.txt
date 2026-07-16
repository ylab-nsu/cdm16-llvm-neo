set(LIBC_SOURCES "")

list(APPEND LIBC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/string.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/stdlib.S)

add_subdirectory(stdbit)

create_cocas_object_library(cocas_libc ${CDM_RT_COCAS_LIB_DIR}/c.lib SRCS ${LIBC_SOURCES})

create_elf_library(elf_libc ${CDM_RT_ELF_LIB_DIR} c SRCS ${LIBC_SOURCES})

