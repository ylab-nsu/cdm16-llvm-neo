set(BUILTIN_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/mulhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/muldi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulosi4.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divdi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/udivhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/udivsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/udivdi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/modhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/modsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/moddi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/umodhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/umodsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/umoddi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/udivmoddi4.S
)

set(BUILTIN_OBJECTS "")

foreach(src ${BUILTIN_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    set(obj ${CMAKE_CURRENT_BINARY_DIR}/${name}.o)
    list(APPEND BUILTIN_OBJECTS ${obj})

    add_custom_command(OUTPUT ${obj}
        COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}" ${CMAKE_ASM_COMPILER} -target cdm-cocas -c -o ${obj} ${src}
        DEPENDS ${src} ${CMAKE_CURRENT_SOURCE_DIR}/cdm_asm.inc)
endforeach()

add_custom_command(OUTPUT ${CDM_RT_COCAS_LIB_DIR}/cdm-builtins.lib
    COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}" ${CMAKE_ASM_COMPILER} -target cdm-cocas -r -o ${CDM_RT_COCAS_LIB_DIR}/cdm-builtins.lib ${BUILTIN_OBJECTS}
    DEPENDS ${BUILTIN_OBJECTS})

add_custom_target(cdm_builtins ALL DEPENDS ${CDM_RT_COCAS_LIB_DIR}/cdm-builtins.lib)
