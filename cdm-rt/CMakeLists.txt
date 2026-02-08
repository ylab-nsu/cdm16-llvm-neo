cmake_minimum_required(VERSION 3.20.0)
set(LLVM_SUBPROJECT_TITLE "cdm-rt")

# Check --print-resource-dir to find the compiler resource dir
execute_process(
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND ${CMAKE_C_COMPILER} --print-resource-dir
  RESULT_VARIABLE COMMAND_RETURN_CODE
  OUTPUT_VARIABLE COMPILER_RESOURCE_DIR
)
if(COMMAND_RETURN_CODE EQUAL 0)
  set(COMPILER_RESOURCE_DIR
    "${COMPILER_RESOURCE_DIR}" CACHE PATH "path to compiler resource dir"
  )
  message(STATUS "Set COMPILER_RESOURCE_DIR to "
                 ${COMPILER_RESOURCE_DIR})
else()
  message(FATAL_ERROR "Failed to get compiler resource dir")
endif()

set(CDM_COCAS_RT_LIB_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm-unknown-unknown-cocas")
set(CDM_ELF_RT_LIB_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm")

file(MAKE_DIRECTORY ${CDM_COCAS_RT_LIB_DIR})
file(MAKE_DIRECTORY ${CDM_ELF_RT_LIB_DIR})

add_subdirectory(cdm-cocas)
#add_subdirectory(cdm-elf)
