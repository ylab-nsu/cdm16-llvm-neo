
# Create objects library with cocas
# USAGE: create_object_library(target output SRCS <sources...>)
function(create_object_library target output)
  cmake_parse_arguments(
    "CREATE_OBJECT_LIBRARY"
    "" # optional arguments
    "" # Single value arguments
    "SRCS" # Multivalue arguments
    ${ARGN}
  )

  add_library(${target}-objects OBJECT ${CREATE_OBJECT_LIBRARY_SRCS})
  target_compile_options(${target}-objects PRIVATE -target cdm-cocas)

  add_custom_command(OUTPUT ${output}
                     COMMAND ${COCAS_EXECUTABLE} -m -o ${output} $<TARGET_OBJECTS:${target}-objects>
                     DEPENDS ${target}-objects)
  add_custom_target(${target} ALL DEPENDS ${output})

endfunction(create_object_library)

