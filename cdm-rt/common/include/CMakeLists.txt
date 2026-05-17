add_subdirectory(cdm)

configure_file(string.h ${CDM_RT_INC_DIR}/string.h COPYONLY)
configure_file(stdlib.h ${CDM_RT_INC_DIR}/stdlib.h COPYONLY)
configure_file(stdbit.h ${CDM_RT_INC_DIR}/stdbit.h COPYONLY)
