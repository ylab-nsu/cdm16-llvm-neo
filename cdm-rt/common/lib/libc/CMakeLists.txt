
create_object_library(cocas_libc ${CDM_RT_COCAS_LIB_DIR}/libc.o
                      SRCS ${CMAKE_CURRENT_SOURCE_DIR}/string.S)

add_library(elf_libc STATIC ${CMAKE_CURRENT_SOURCE_DIR}/string.S)
target_compile_options(elf_libc PRIVATE -g0)
set_target_properties(elf_libc PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${CDM_RT_ELF_LIB_DIR}
                      OUTPUT_NAME "c")
