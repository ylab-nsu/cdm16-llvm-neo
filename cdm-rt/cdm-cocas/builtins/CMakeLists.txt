set(BUILDIN_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/mulhi3.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/divhi3.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/modhi3.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/udivhi3.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/umodhi3.asm
)

create_object_library(target_name ${CDM_COCAS_RT_LIB_DIR}/cdm-builtins.lib SRCS ${BUILDIN_SOURCES})

