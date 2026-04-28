
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

  get_directory_property(INC_DIRS INCLUDE_DIRECTORIES)

  set(INC_FLAGS "")
  foreach(DIR IN LISTS INC_DIRS)
      list(APPEND INC_FLAGS "-I${DIR}")
  endforeach()

  add_custom_command(OUTPUT ${output}
                     COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}"
                     ${CMAKE_ASM_COMPILER} -target cdm-cocas ${INC_FLAGS} -r -o ${output} ${CREATE_OBJECT_LIBRARY_SRCS})
  add_custom_target(${target} ALL DEPENDS ${output})

endfunction(create_object_library)

