
add_custom_command(
    OUTPUT ${CDM_COCAS_RT_LIB_DIR}/crt0.o
    COMMAND ${CMAKE_C_COMPILER} -target cdm-cocas
                                -c
                                -o ${CDM_COCAS_RT_LIB_DIR}/crt0.o
                                crt0.asm
    DEPENDS crt0.asm
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM)


add_custom_target(cdm_rt_cocs ALL DEPENDS
    ${COMPILER_RESOURCE_DIR}/lib/cdm-unknown-unknown-cocas/crt0.o)
