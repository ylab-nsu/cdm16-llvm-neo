create_elf_object_library(elf_crt0 ${CDM_RT_ELF_LIB_DIR} crt0 SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0.s)
configure_file(cdm.ld ${CDM_RT_ELF_LIB_DIR}/ldscripts/cdm.ld COPYONLY)
