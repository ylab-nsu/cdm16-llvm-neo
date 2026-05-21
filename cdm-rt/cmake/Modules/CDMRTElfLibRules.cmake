
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

  add_library(${target}_objects OBJECT ${CREATE_ELF_LIBRARY_SRCS})
  target_compile_options(${target}_objects
                         PRIVATE -g0
                         PRIVATE --target=cdm)

  add_custom_command(
      OUTPUT ${output_dir}/lib${name}.a
      COMMAND ${LLVM_BINARY_DIR}/bin/llvm-ar -rcs ${output_dir}/lib${name}.a $<TARGET_OBJECTS:${target}_objects>
      DEPENDS ${target}_objects
      COMMAND_EXPAND_LISTS
      VERBATIM
  )

  add_custom_target(${target} ALL DEPENDS ${output_dir}/lib${name}.a)

endfunction(create_elf_library)

