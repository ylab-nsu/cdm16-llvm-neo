
# Create static library for elf target
# USAGE: create_elf_library(target output_dir name SRCS <sources...>)
function(create_elf_library target output_dir name)
  cmake_parse_arguments(
    "CREATE_ELF_LIBRARY"
    "" # optional arguments
    "" # Single value arguments
    "SRCS" # Multivalue arguments
    ${ARGN}
  )

  add_library(${target} STATIC ${CREATE_ELF_LIBRARY_SRCS})
  target_compile_options(${target}
                         PRIVATE -g0
                         PRIVATE --target=cdm)
  set_target_properties(${target} PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY ${output_dir}
                        OUTPUT_NAME ${name})

endfunction(create_elf_library)

