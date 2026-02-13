list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(CDMRTObjectRules)
find_package(Cocas REQUIRED)

if (NOT COCAS_FOUND)
    message(SEND_ERROR "Cocas not found")
else()
    message(STATUS "Cocas found: ${COCAS_EXECUTABLE}")
endif()

create_object_library(cocas_crt0 ${CDM_COCAS_RT_LIB_DIR}/crt0.o SRCS ${CMAKE_CURRENT_SOURCE_DIR}/crt0/crt0.asm)
