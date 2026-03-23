find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TEST_CDM_VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.venv")
set(TEST_CDM_REQUIREMENTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")

if (WIN32)
    set(VENV_BIN_DIR_NAME "Scripts")
else()
    set(VENV_BIN_DIR_NAME "bin")
endif()

# Set up venv if it is not set up yet
if (NOT EXISTS ${TEST_CDM_VENV_DIR})
    message(STATUS "Setting up venv for test_cdm")

    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m venv ${TEST_CDM_VENV_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )

    message(STATUS "Installing dependencies for test_cdm")
    execute_process(
        COMMAND ${TEST_CDM_VENV_DIR}/${VENV_BIN_DIR_NAME}/pip install -r ${TEST_CDM_REQUIREMENTS_PATH}
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/test_cdm.py.in ${LLVM_BINARY_DIR}/bin/test_cdm)
