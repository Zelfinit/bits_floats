#include "assignment.h"

#include <math.h>
#include <stddef.h>

unsigned bitset_symmetric_difference_count(uint32_t a, uint32_t b) {
    uint32_t c = a ^ b;
    int count = 0;
    while(c){
        count += c & 1;
        c >>= 1;
    }
    return count;
}

uint32_t bit_range_mask(unsigned low, unsigned high) {
    int range = high - low + 1;
    uint32_t mask = 0;
    for (int i = 0; i < range; i++){
        mask <<= 1;
        mask |= 1;
    }
    for (unsigned i = 0; i < low; i++){
        mask <<= 1;
    }
    return mask;
}

uint32_t splice_bits(
    uint32_t destination,
    uint32_t source,
    unsigned low,
    unsigned high
) {
    uint32_t mask = bit_range_mask(low, high);
    uint32_t result = destination & ~mask;
    result |= (source << low) & mask;
    return result;
}

int32_t decode_twos_complement(uint32_t bits, unsigned width) {
    uint32_t mask = bit_range_mask(0,width-1);
    uint32_t num = bits & mask;
    if (num >> (width - 1) & 1){
        num |= ~mask;
    }
    return num;
}

bool add_signed_width(
    uint32_t a_bits,
    uint32_t b_bits,
    unsigned width,
    uint32_t *sum_bits
) {
    (void)a_bits;
    (void)b_bits;
    (void)width;

    if (sum_bits != NULL) {
        *sum_bits = 0;
    }
    return false;
}

int compare_signed_unsigned_math(
    int32_t signed_value,
    uint32_t unsigned_value
) {
    (void)signed_value;
    (void)unsigned_value;
    return 0;
}

uint32_t multiply_by_24(uint32_t x) {
    (void)x;
    return 0;
}

fp32_info_t fp32_decode(uint32_t bits) {
    (void)bits;

    fp32_info_t info = {
        .negative = false,
        .kind = FP32_ZERO,
        .exponent_bits = 0,
        .fraction_bits = 0,
        .unbiased_exponent = -126,
    };
    return info;
}

double fp32_exact_value(uint32_t bits) {
    (void)bits;
    return 0.0;
}

uint32_t fp32_next_up_bits(uint32_t bits) {
    (void)bits;
    return 0;
}

bool fp32_truncate_to_int(uint32_t bits, int32_t *out) {
    (void)bits;

    if (out != NULL) {
        *out = 0;
    }
    return false;
}
