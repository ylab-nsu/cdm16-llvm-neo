set(BUILTIN_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/mulhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/muldi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulosi4.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divmodhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divmodsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divmoddi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/adddi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/subdi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/negdi2.S
)

set(ELF_BUILTIN_OBJECTS "")

foreach(src ${BUILTIN_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    set(obj ${CMAKE_CURRENT_BINARY_DIR}/${name}.o)
    list(APPEND ELF_BUILTIN_OBJECTS ${obj})

    add_custom_command(OUTPUT ${obj}
        COMMAND ${CMAKE_ASM_COMPILER} -target cdm -c -o ${obj} ${src}
        DEPENDS ${src} ${CMAKE_CURRENT_SOURCE_DIR}/cdm_asm.inc)
endforeach()

add_custom_command(OUTPUT ${CDM_RT_ELF_RT_BUILTINS_DIR}/clang_rt.cdm-builtins.o
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CDM_RT_ELF_RT_BUILTINS_DIR}/clang_rt.cdm-builtins.o
    COMMAND ${CMAKE_AR} rcs ${CDM_RT_ELF_RT_BUILTINS_DIR}/clang_rt.cdm-builtins.o ${ELF_BUILTIN_OBJECTS}
    DEPENDS ${ELF_BUILTIN_OBJECTS})

add_custom_target(cdm_builtins_elf ALL DEPENDS ${CDM_RT_ELF_RT_BUILTINS_DIR}/clang_rt.cdm-builtins.o)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cocas)

set(COCAS_BUILTIN_OBJECTS "")

foreach(src ${BUILTIN_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    set(obj ${CMAKE_CURRENT_BINARY_DIR}/cocas/${name}.o)
    list(APPEND COCAS_BUILTIN_OBJECTS ${obj})

    add_custom_command(OUTPUT ${obj}
        COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}" ${CMAKE_ASM_COMPILER} -target cdm-cocas -c -o ${obj} ${src}
        DEPENDS ${src} ${CMAKE_CURRENT_SOURCE_DIR}/cdm_asm.inc)
endforeach()

add_custom_command(OUTPUT ${CDM_RT_COCAS_BUILTINS_DIR}/clang_rt.cdm-builtins.o
    COMMAND ${CMAKE_COMMAND} -E env COCAS="${COCAS_EXECUTABLE}" ${CMAKE_ASM_COMPILER} -target cdm-cocas -r -o ${CDM_RT_COCAS_BUILTINS_DIR}/clang_rt.cdm-builtins.o ${COCAS_BUILTIN_OBJECTS}
    DEPENDS ${COCAS_BUILTIN_OBJECTS})

add_custom_target(cdm_builtins ALL DEPENDS ${CDM_RT_COCAS_BUILTINS_DIR}/clang_rt.cdm-builtins.o)
