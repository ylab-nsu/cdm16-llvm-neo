find_package(Cocas REQUIRED)

if (NOT COCAS_FOUND)
    message(SEND_ERROR "Cocas not found")
else()
    message(STATUS "Cocas found: ${COCAS_EXECUTABLE}")
endif()

add_subdirectory(crt0)
