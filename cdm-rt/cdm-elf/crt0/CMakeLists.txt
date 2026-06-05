create_elf_object_library(elf_crt0_vonneumann ${CDM_RT_ELF_LIB_DIR} crt0-vonNeumann SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0-vonNeumann.s)
create_elf_object_library(elf_crt0_harvard ${CDM_RT_ELF_LIB_DIR} crt0-harvard SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0-harvard.s)
