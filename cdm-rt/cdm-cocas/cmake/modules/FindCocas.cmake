# FindCocas
# -----------
#
# Find cocas executable
#
# The module defines the following variables:
#   COCAS_FOUND          - true if cocas was found
#   COCAS_EXECUTABLE     - path to cocas executable

find_program(COCAS_EXECUTABLE cocas)

if ("${COCAS_EXECUTABLE}" STREQUAL "COCAS_EXECUTABLE-NOTFOUND")
    set(COCAS_FOUND FALSE)
else()
    set(COCAS_FOUND TRUE)
endif()
