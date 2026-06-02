create_elf_object_library(elf_crt0_vn ${CDM_RT_ELF_LIB_DIR} crt0-vn SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0-vn.s)
create_elf_object_library(elf_crt0_hv ${CDM_RT_ELF_LIB_DIR} crt0-hv SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0-hv.s)
configure_file(cdm-vn.ld ${CDM_RT_ELF_LIB_DIR}/ldscripts/cdm-vn.ld COPYONLY)
configure_file(cdm-hv.ld ${CDM_RT_ELF_LIB_DIR}/ldscripts/cdm-hv.ld COPYONLY)
