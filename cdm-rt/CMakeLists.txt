cmake_minimum_required(VERSION 3.20.0)
set(LLVM_SUBPROJECT_TITLE "cdm-rt")

include(GetClangResourceDir)

get_clang_resource_dir(COMPILER_RESOURCE_DIR)
set(COMPILER_RESOURCE_DIR ${LLVM_BINARY_DIR}/${COMPILER_RESOURCE_DIR})
message(STATUS "Set COMPILER_RESOURCE_DIR to "
             ${COMPILER_RESOURCE_DIR})

set(CDM_COCAS_RT_LIB_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm-unknown-unknown-cocas")
set(CDM_ELF_RT_LIB_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm")

file(MAKE_DIRECTORY ${CDM_COCAS_RT_LIB_DIR})
file(MAKE_DIRECTORY ${CDM_ELF_RT_LIB_DIR})

add_subdirectory(cdm-cocas)
#add_subdirectory(cdm-elf)
