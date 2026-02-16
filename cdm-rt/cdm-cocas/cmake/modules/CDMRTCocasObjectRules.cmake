
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

  add_custom_command(OUTPUT ${output}
                     COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}"
                     ${CMAKE_ASM_COMPILER} -target cdm-cocas -r -o ${output} ${CREATE_OBJECT_LIBRARY_SRCS})
  add_custom_target(${target} ALL DEPENDS ${output})

endfunction(create_object_library)

