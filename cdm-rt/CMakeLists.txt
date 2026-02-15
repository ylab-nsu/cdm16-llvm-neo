cmake_minimum_required(VERSION 3.20.0)
set(LLVM_SUBPROJECT_TITLE "cdm-rt")

include(GetClangResourceDir)

get_clang_resource_dir(COMPILER_RESOURCE_DIR)
set(COMPILER_RESOURCE_DIR ${LLVM_BINARY_DIR}/${COMPILER_RESOURCE_DIR})
message(STATUS "Set COMPILER_RESOURCE_DIR to "
             ${COMPILER_RESOURCE_DIR})

# Set builtins dirs
set(CDM_RT_COCAS_BUILTINS_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm-unknown-unknown-cocas")
set(CDM_RT_ELF_RT_BUILTINS_DIR "${COMPILER_RESOURCE_DIR}/lib/cdm")

# Set stdlib dirs
set(CDM_RT_COCAS_LIB_DIR "${LLVM_BINARY_DIR}/lib/cdm-unknown-unknown-cocas")
set(CDM_RT_ELF_LIB_DIR "${LLVM_BINARY_DIR}/lib/cdm")

# Set headers dir
set(CDM_RT_INC_DIR "${LLVM_BINARY_DIR}/include/cdm")

file(MAKE_DIRECTORY ${CDM_RT_COCAS_BUILTINS_DIR})
file(MAKE_DIRECTORY ${CDM_RT_ELF_RT_BUILTINS_DIR})
file(MAKE_DIRECTORY ${CDM_RT_COCAS_LIB_DIR})
file(MAKE_DIRECTORY ${CDM_RT_ELF_LIB_DIR})
file(MAKE_DIRECTORY ${CDM_RT_INC_DIR})

add_subdirectory(cdm-cocas)
#add_subdirectory(cdm-elf)
#add_subdirectory(common)
