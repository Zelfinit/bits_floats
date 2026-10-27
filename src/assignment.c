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
    uint32_t mask = bit_range_mask(0,width-1);
    uint32_t a_num = a_bits & mask;
    uint32_t b_num = b_bits & mask;
    uint32_t sum = (a_num + b_num) & mask;

    if (sum_bits != NULL) {
        *sum_bits = sum;
    }
    if (a_num>>(width-1)==b_num>>(width-1) && a_num>>(width-1)!=sum>>(width-1)){
        return true;
    }
    return false;
}

int compare_signed_unsigned_math(
    int32_t signed_value,
    uint32_t unsigned_value
) {
    if (signed_value<0){
        return -1;
    }
    uint32_t unsign = (uint32_t) signed_value;
    if(unsign==unsigned_value){
        return 0;
    }
    if (unsigned_value > unsign){
    return -1;
    }else{
    return 1;
}
}

uint32_t multiply_by_24(uint32_t x) {
    return (x<<5)-(x<<3);
}

fp32_info_t fp32_decode(uint32_t bits) {
    bool isNegative = bits >> 31;
    uint32_t exponents = splice_bits(0, bits>>23, 0, 7);
    uint32_t mantissa = splice_bits(0, bits, 0, 22);
    fp32_kind_t type = FP32_NORMAL;
    int unbiased = (int)exponents - 127;
    int mant0 = compare_signed_unsigned_math(0, mantissa);
    int exp0 = compare_signed_unsigned_math(0,exponents);
    int exp255 = compare_signed_unsigned_math(255, exponents);

    if (exp0 == 0){
        unbiased = -126;
        if(mant0 == 0){
            type = FP32_ZERO;
        }else{
            type = FP32_SUBNORMAL;
        }
    }
    if (exp255 == 0){
        unbiased = 0;
        if(mant0==0){
            type = FP32_INFINITY;
        }else{
            type = FP32_NAN;
        }
    }
    fp32_info_t info = {
        .negative = isNegative,
        .kind = type,
        .exponent_bits = (uint8_t) exponents,
        .fraction_bits = mantissa,
        .unbiased_exponent = unbiased,
    };
    return info;
}

double fp32_exact_value(uint32_t bits) {
    fp32_info_t info = fp32_decode(bits);
    int sign = info.negative ? -1.0 : 1.0;

    switch(info.kind) {
    case FP32_ZERO:
        return info.negative ? -0.0 : 0.0;
    case FP32_INFINITY:
        return info.negative ? -INFINITY : INFINITY;
    case FP32_NAN:
        return NAN;
    case FP32_SUBNORMAL:
        return sign * ldexp((double)info.fraction_bits, info.unbiased_exponent - 23);
    default:
        return sign * ldexp((double)((1u << 23) + info.fraction_bits), info.unbiased_exponent - 23);
    }
}

uint32_t fp32_next_up_bits(uint32_t bits) {
    fp32_info_t info = fp32_decode(bits);
    switch(info.kind){
    case FP32_NAN:
        return bits;
    case FP32_INFINITY:
        if(info.negative){
            return bits-1;
        }else{
            return bits;
        }
    case FP32_ZERO:
        return 1u;
    default:
        if(info.negative){
            return bits-1;
        }else{
            return bits+1;
        }
    }
}

bool fp32_truncate_to_int(uint32_t bits, int32_t *out) {
    fp32_info_t info = fp32_decode(bits);
    if (info.kind == FP32_NAN || info.kind == FP32_INFINITY){
        return false;
    }
    if(info.kind == FP32_SUBNORMAL || info.unbiased_exponent<0){
        *out = 0;
        return true;
    }
    if(info.unbiased_exponent>31){
        return false;
    }
    if(info.unbiased_exponent==31){
        if(info.negative && info.fraction_bits==0){
            *out = INT32_MIN;
            return true;
        }
        return false;
    }
    uint32_t significand = (1u<<23) | info.fraction_bits;
    if(info.unbiased_exponent>=23){
        significand = significand << (info.unbiased_exponent-23);
    }else{
        significand = significand >> (23-info.unbiased_exponent);
    }
    *out = info.negative ? - (int32_t) significand : (int32_t) significand;
    
    return true;
}
