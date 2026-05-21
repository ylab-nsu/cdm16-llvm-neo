#ifndef _STDBIT_H
#define _STDBIT_H

#define __need_size_t
#include <stddef.h>

#include <stdbool.h>
#include <stdint.h>

#define __STDC_VERSION_STDBIT_H__	202311L

#define __STDC_ENDIAN_LITTLE__ 1234
#define __STDC_ENDIAN_BIG__ 4321
#define __STDC_ENDIAN_NATIVE__ __STDC_ENDIAN_LITTLE__

unsigned int stdc_leading_zeros_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_leading_zeros_us(unsigned short value) [[unsequenced]];
unsigned int stdc_leading_zeros_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_leading_zeros_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_leading_zeros_ull(unsigned long long int value) [[unsequenced]];

#define stdc_leading_zeros(x) _Generic((x), \
        unsigned char: stdc_leading_zeros_uc, \
        unsigned short: stdc_leading_zeros_us, \
        unsigned int: stdc_leading_zeros_ui, \
        unsigned long int: stdc_leading_zeros_ul, \
        unsigned long long int: stdc_leading_zeros_ull \
)(x)

unsigned int stdc_leading_ones_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_leading_ones_us(unsigned short value) [[unsequenced]];
unsigned int stdc_leading_ones_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_leading_ones_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_leading_ones_ull(unsigned long long int value) [[unsequenced]];

#define stdc_leading_ones(x) _Generic((x), \
        unsigned char: stdc_leading_ones_uc, \
        unsigned short: stdc_leading_ones_us, \
        unsigned int: stdc_leading_ones_ui, \
        unsigned long int: stdc_leading_ones_ul, \
        unsigned long long int: stdc_leading_ones_ull \
)(x)

unsigned int stdc_trailing_zeros_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_trailing_zeros_us(unsigned short value) [[unsequenced]];
unsigned int stdc_trailing_zeros_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_trailing_zeros_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_trailing_zeros_ull(unsigned long long int value) [[unsequenced]];

#define stdc_trailing_zeros(x) _Generic((x), \
        unsigned char: stdc_trailing_zeros_uc, \
        unsigned short: stdc_trailing_zeros_us, \
        unsigned int: stdc_trailing_zeros_ui, \
        unsigned long int: stdc_trailing_zeros_ul, \
        unsigned long long int: stdc_trailing_zeros_ull \
)(x)

unsigned int stdc_trailing_ones_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_trailing_ones_us(unsigned short value) [[unsequenced]];
unsigned int stdc_trailing_ones_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_trailing_ones_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_trailing_ones_ull(unsigned long long int value) [[unsequenced]];

#define stdc_trailing_ones(x) _Generic((x), \
        unsigned char: stdc_trailing_ones_uc, \
        unsigned short: stdc_trailing_ones_us, \
        unsigned int: stdc_trailing_ones_ui, \
        unsigned long int: stdc_trailing_ones_ul, \
        unsigned long long int: stdc_trailing_ones_ull \
)(x)

unsigned int stdc_first_leading_zero_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_first_leading_zero_us(unsigned short value) [[unsequenced]];
unsigned int stdc_first_leading_zero_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_first_leading_zero_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_first_leading_zero_ull(unsigned long long int value) [[unsequenced]];

#define stdc_first_leading_zero(x) _Generic((x), \
        unsigned char: stdc_first_leading_zero_uc, \
        unsigned short: stdc_first_leading_zero_us, \
        unsigned int: stdc_first_leading_zero_ui, \
        unsigned long int: stdc_first_leading_zero_ul, \
        unsigned long long int: stdc_first_leading_zero_ull \
)(x)

unsigned int stdc_first_leading_one_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_first_leading_one_us(unsigned short value) [[unsequenced]];
unsigned int stdc_first_leading_one_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_first_leading_one_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_first_leading_one_ull(unsigned long long int value) [[unsequenced]];

#define stdc_first_leading_one(x) _Generic((x), \
        unsigned char: stdc_first_leading_one_uc, \
        unsigned short: stdc_first_leading_one_us, \
        unsigned int: stdc_first_leading_one_ui, \
        unsigned long int: stdc_first_leading_one_ul, \
        unsigned long long int: stdc_first_leading_one_ull \
)(x)

unsigned int stdc_first_trailing_zero_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_first_trailing_zero_us(unsigned short value) [[unsequenced]];
unsigned int stdc_first_trailing_zero_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_first_trailing_zero_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_first_trailing_zero_ull(unsigned long long int value) [[unsequenced]];

#define stdc_first_trailing_zero(x) _Generic((x), \
        unsigned char: stdc_first_trailing_zero_uc, \
        unsigned short: stdc_first_trailing_zero_us, \
        unsigned int: stdc_first_trailing_zero_ui, \
        unsigned long int: stdc_first_trailing_zero_ul, \
        unsigned long long int: stdc_first_trailing_zero_ull \
)(x)

unsigned int stdc_first_trailing_one_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_first_trailing_one_us(unsigned short value) [[unsequenced]];
unsigned int stdc_first_trailing_one_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_first_trailing_one_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_first_trailing_one_ull(unsigned long long int value) [[unsequenced]];

#define stdc_first_trailing_one(x) _Generic((x), \
        unsigned char: stdc_first_trailing_one_uc, \
        unsigned short: stdc_first_trailing_one_us, \
        unsigned int: stdc_first_trailing_one_ui, \
        unsigned long int: stdc_first_trailing_one_ul, \
        unsigned long long int: stdc_first_trailing_one_ull \
)(x)

unsigned int stdc_count_zeros_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_count_zeros_us(unsigned short value) [[unsequenced]];
unsigned int stdc_count_zeros_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_count_zeros_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_count_zeros_ull(unsigned long long int value) [[unsequenced]];

#define stdc_count_zeros(x) _Generic((x), \
        unsigned char: stdc_count_zeros_uc, \
        unsigned short: stdc_count_zeros_us, \
        unsigned int: stdc_count_zeros_ui, \
        unsigned long int: stdc_count_zeros_ul, \
        unsigned long long int: stdc_count_zeros_ull \
)(x)

unsigned int stdc_count_ones_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_count_ones_us(unsigned short value) [[unsequenced]];
unsigned int stdc_count_ones_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_count_ones_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_count_ones_ull(unsigned long long int value) [[unsequenced]];

#define stdc_count_ones(x) _Generic((x), \
        unsigned char: stdc_count_ones_uc, \
        unsigned short: stdc_count_ones_us, \
        unsigned int: stdc_count_ones_ui, \
        unsigned long int: stdc_count_ones_ul, \
        unsigned long long int: stdc_count_ones_ull \
)(x)

bool stdc_has_single_bit_uc(unsigned char value) [[unsequenced]];
bool stdc_has_single_bit_us(unsigned short value) [[unsequenced]];
bool stdc_has_single_bit_ui(unsigned int value) [[unsequenced]];
bool stdc_has_single_bit_ul(unsigned long int value) [[unsequenced]];
bool stdc_has_single_bit_ull(unsigned long long int value) [[unsequenced]];

#define stdc_has_single_bit(x) _Generic((x), \
        unsigned char: stdc_has_single_bit_uc, \
        unsigned short: stdc_has_single_bit_us, \
        unsigned int: stdc_has_single_bit_ui, \
        unsigned long int: stdc_has_single_bit_ul, \
        unsigned long long int: stdc_has_single_bit_ull \
)(x)

unsigned int stdc_bit_width_uc(unsigned char value) [[unsequenced]];
unsigned int stdc_bit_width_us(unsigned short value) [[unsequenced]];
unsigned int stdc_bit_width_ui(unsigned int value) [[unsequenced]];
unsigned int stdc_bit_width_ul(unsigned long int value) [[unsequenced]];
unsigned int stdc_bit_width_ull(unsigned long long int value) [[unsequenced]];

#define stdc_bit_width(x) _Generic((x), \
        unsigned char: stdc_bit_width_uc, \
        unsigned short: stdc_bit_width_us, \
        unsigned int: stdc_bit_width_ui, \
        unsigned long int: stdc_bit_width_ul, \
        unsigned long long int: stdc_bit_width_ull \
)(x)

unsigned char stdc_bit_floor_uc(unsigned char value) [[unsequenced]];
unsigned short stdc_bit_floor_us(unsigned short value) [[unsequenced]];
unsigned int stdc_bit_floor_ui(unsigned int value) [[unsequenced]];
unsigned long int stdc_bit_floor_ul(unsigned long int value) [[unsequenced]];
unsigned long long int stdc_bit_floor_ull(unsigned long long int value) [[unsequenced]];

#define stdc_bit_floor(x) _Generic((x), \
        unsigned char: stdc_bit_floor_uc, \
        unsigned short: stdc_bit_floor_us, \
        unsigned int: stdc_bit_floor_ui, \
        unsigned long int: stdc_bit_floor_ul, \
        unsigned long long int: stdc_bit_floor_ull \
)(x)

unsigned char stdc_bit_ceil_uc(unsigned char value) [[unsequenced]];
unsigned short stdc_bit_ceil_us(unsigned short value) [[unsequenced]];
unsigned int stdc_bit_ceil_ui(unsigned int value) [[unsequenced]];
unsigned long int stdc_bit_ceil_ul(unsigned long int value) [[unsequenced]];
unsigned long long int stdc_bit_ceil_ull(unsigned long long int value) [[unsequenced]];

#define stdc_bit_ceil(x) _Generic((x), \
        unsigned char: stdc_bit_ceil_uc, \
        unsigned short: stdc_bit_ceil_us, \
        unsigned int: stdc_bit_ceil_ui, \
        unsigned long int: stdc_bit_ceil_ul, \
        unsigned long long int: stdc_bit_ceil_ull \
)(x)

#endif
