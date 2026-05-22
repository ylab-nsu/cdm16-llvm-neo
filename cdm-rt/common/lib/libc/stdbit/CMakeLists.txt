list(APPEND LIBC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/bit_ceil.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/bit_floor.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/bit_width.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/count_ones.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/count_zeros.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/first_leading_one.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/first_leading_zero.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/first_trailing_one.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/first_trailing_zero.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/has_single_bit.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/leading_ones.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/leading_zeros.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/trailing_ones.S
                         ${CMAKE_CURRENT_SOURCE_DIR}/trailing_zeros.S)

set(LIBC_SOURCES ${LIBC_SOURCES} PARENT_SCOPE)

