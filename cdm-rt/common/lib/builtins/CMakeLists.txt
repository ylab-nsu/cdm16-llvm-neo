set(BUILTIN_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/mulhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mulosi4.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divmodhi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/divmodsi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/adddi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/subdi3.S
  ${CMAKE_CURRENT_SOURCE_DIR}/negdi2.S
)

set(BUILTIN_C_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/muldi3.c
  ${CMAKE_CURRENT_SOURCE_DIR}/divmoddi3.c
)

list(APPEND ALL_BUILTIN_SOURCES ${BUILTIN_SOURCES} ${BUILTIN_C_SOURCES})

create_cocas_object_library(cdm_builtins 
    ${CDM_RT_COCAS_BUILTINS_DIR}/clang_rt.cdm-builtins.o 
    SRCS ${ALL_BUILTIN_SOURCES}
)

create_elf_library(cdm_builtins_elf 
    ${CDM_RT_ELF_RT_BUILTINS_DIR} 
    clang_rt.cdm-builtins 
    SRCS ${ALL_BUILTIN_SOURCES}
)