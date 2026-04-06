find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TEST_CDM_VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.venv")
set(TEST_CDM_REQUIREMENTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")

set(TEST_CDM_FILE_NAME "test_cdm")
if (WIN32)
    set(VENV_BIN_DIR_NAME "Scripts")
    set(TEST_CDM_FILE_NAME "${TEST_CDM_FILE_NAME}.py")
else()
    set(VENV_BIN_DIR_NAME "bin")
endif()

# Set up venv
add_custom_command(OUTPUT ${TEST_CDM_VENV_DIR}
                   COMMAND ${Python3_EXECUTABLE} -m venv ${TEST_CDM_VENV_DIR}
                   COMMAND ${TEST_CDM_VENV_DIR}/${VENV_BIN_DIR_NAME}/pip install -q -r ${TEST_CDM_REQUIREMENTS_PATH}
                   DEPENDS ${TEST_CDM_REQUIREMENTS_PATH})
add_custom_target(test_cdm_setup_venv ALL
                  DEPENDS ${TEST_CDM_VENV_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_cdm/test_cdm.py.in ${LLVM_BINARY_DIR}/bin/${TEST_CDM_FILE_NAME})
