
create_cocas_object_library(cocas_libc ${CDM_RT_COCAS_LIB_DIR}/libc.o
                            SRCS ${CMAKE_CURRENT_SOURCE_DIR}/string.S
                                 ${CMAKE_CURRENT_SOURCE_DIR}/stdlib.S
                                 ${CMAKE_CURRENT_SOURCE_DIR}/stdbit.S)

create_elf_library(elf_libc ${CDM_RT_ELF_LIB_DIR} c SRCS ${CMAKE_CURRENT_SOURCE_DIR}/string.S
                                                         ${CMAKE_CURRENT_SOURCE_DIR}/stdlib.S
                                                         ${CMAKE_CURRENT_SOURCE_DIR}/stdbit.S)

