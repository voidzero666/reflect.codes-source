#pragma once

#include <stdint.h>

#ifndef WIN32
    #include <stddef.h>
#endif

extern void xor_crypt(uint8_t* p_data, size_t data_size, const uint8_t* p_key, size_t key_size, uintptr_t data_offset, uintptr_t key_offset)
{
    uintptr_t j = key_offset;
    for(uint8_t* p_data_it = p_data + data_offset, * p_data_end = p_data + data_size; p_data_it != p_data_end; ++p_data_it, ++j)
        (*p_data_it) ^= p_key[j % key_size];
}

#include <time.h>

#define time_ranged_seed(base, range) (((time(NULL) / base) - range) / range / 2)

#include <stdint.h>

#ifndef WIN32
    #include <stddef.h>
#endif

extern void tx_crypt(uint8_t* p_data, size_t data_size, const uint8_t* p_key, size_t key_size, time_t time_range, uintptr_t data_offset, uintptr_t key_offset)
{
    uintptr_t j = key_offset + ((time_range == 0) ? 0 : time_ranged_seed(60, time_range));
    for(uint8_t* p_data_it = p_data + data_offset, * p_data_end = p_data + data_size; p_data_it != p_data_end; ++p_data_it, ++j)
        (*p_data_it) ^= p_key[j % key_size] ^ (uint8_t)j;
}

#include <stdint.h>
#include <string.h>

#ifndef WIN32
    #include <stddef.h>
#endif

extern void compress_key(const uint8_t* p_src, size_t src_size, uint8_t* p_dst, size_t dst_size)
{
    memset(p_dst, '\0', dst_size);
    for(uintptr_t i = 0; i < src_size; i++)
    {
        uint8_t src_byte = p_src[i],
                dst_byte = p_dst[i % dst_size];
        if((dst_byte == 0) || (dst_byte != src_byte))
            p_dst[i % dst_size] ^= src_byte;
    }
}

#include <stdint.h>

#ifdef WIN32
    #include <time.h>
#else
    #include <stddef.h>
    #include <sys/time.h>
#endif

#if defined(__MINGW32__) && !defined(gettimeofday)
    #define gettimeofday mingw_gettimeofday
#endif

static uint64_t xorshift_seed = 1;

extern uint64_t xorshift_from(uint64_t seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

extern uint64_t xorshift_next()
{
    return xorshift_seed = xorshift_from(xorshift_seed);
}

extern void xorshift_reset_seed()
{
    xorshift_seed = xorshift_from(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

#include <stdlib.h>
#include <string.h>

struct s_multi_xorshift_seed {
    uint64_t* p_raw;
    size_t length;
};

extern void multi_xorshift_init(struct s_multi_xorshift_seed* p_mxs_seed, const uint8_t* p_base, size_t base_size)
{
    size_t length_u64 = (base_size + sizeof(uint64_t) - 1) / sizeof(uint64_t),
           length     = length_u64 * sizeof(uint64_t);
    uint8_t* p_seed_raw = (uint8_t*)malloc(length);
    uint8_t* p_seed_raw_end = p_seed_raw + length;
    for(uint8_t* p_seed_raw_it = p_seed_raw_end - (sizeof(uint64_t) - base_size % sizeof(uint64_t)); p_seed_raw_it != p_seed_raw_end; ++p_seed_raw_it)
        *p_seed_raw_it = 0;
    memcpy(p_seed_raw, p_base, base_size);
    p_mxs_seed->p_raw = (uint64_t*)p_seed_raw;
    p_mxs_seed->length = length_u64;
}

extern uint64_t multi_xorshift_next(struct s_multi_xorshift_seed* p_mxs_seed)
{
    uint64_t result = 0;
    for(uint64_t* p_mxs_seed_raw_it = p_mxs_seed->p_raw, * p_mxs_seed_raw_end = p_mxs_seed->p_raw + p_mxs_seed->length; p_mxs_seed_raw_it != p_mxs_seed_raw_end; ++p_mxs_seed_raw_it)
    {
        uint64_t next_seed = xorshift_from(*p_mxs_seed_raw_it);
        *p_mxs_seed_raw_it = next_seed;
        if((result == 0) || (result != next_seed))
            result ^= next_seed;
    }
    return result;
}

extern void multi_xorshift_destruct(struct s_multi_xorshift_seed* p_mxs_seed)
{
    free(p_mxs_seed->p_raw);
    p_mxs_seed->p_raw = NULL;
    p_mxs_seed->length = 0;
}

extern void expand_key(const uint8_t* p_src, size_t src_size, uint8_t* p_dst, size_t dst_size)
{
    memset(p_dst, '\0', dst_size);
    struct s_multi_xorshift_seed mxs_seed;
    multi_xorshift_init(&mxs_seed, p_src, src_size);
    size_t dst_size_u64_modulo = dst_size % sizeof(uint64_t);
    uint8_t* p_dst_end = p_dst + dst_size;
    uint64_t* p_dst_end_u64 = (uint64_t*)(p_dst_end - dst_size_u64_modulo);
    for(uint64_t* p_dst_it = (uint64_t*)p_dst; p_dst_it != p_dst_end_u64; ++p_dst_it)
        *p_dst_it = multi_xorshift_next(&mxs_seed);
    uint64_t mxs_next_tmp = multi_xorshift_next(&mxs_seed);
    memcpy(p_dst_end_u64, &mxs_next_tmp, dst_size_u64_modulo);
    multi_xorshift_destruct(&mxs_seed);
}

extern void multi_xorshift_shuffle(struct s_multi_xorshift_seed* p_mxs_seed, uint8_t* p_data, size_t data_size)
{
    for(uintptr_t i = 0; i < data_size; ++i)
    {
        uintptr_t j = multi_xorshift_next(p_mxs_seed) % data_size;
        uint8_t t = p_data[i];
        p_data[i] = p_data[j];
        p_data[j] = t;
    }
}

extern void multi_xorshift_deshuffle(struct s_multi_xorshift_seed* p_mxs_seed, uint8_t* p_data, size_t data_size)
{
    uintptr_t* j_map = (uintptr_t*)malloc(data_size * sizeof(uintptr_t)), * j_map_it = j_map;
    for(uintptr_t i = 0; i < data_size; ++i, ++j_map_it)
        *j_map_it = multi_xorshift_next(p_mxs_seed) % data_size;
    uintptr_t i = data_size;
    while(i > 0)
    {
        --i;
        uintptr_t j = j_map[i];
        uint8_t t = p_data[i];
        p_data[i] = p_data[j];
        p_data[j] = t;
    }
    free(j_map);
}

#define xmxss_prepare_key(p_key, key_size, prepared_key); \
    if(key_size > sizeof(prepared_key)) \
        compress_key(p_key, key_size, prepared_key, sizeof(prepared_key)); \
    else if(key_size < sizeof(prepared_key)) \
        expand_key(p_key, key_size, prepared_key, sizeof(prepared_key)); \
    else \
        memcpy(prepared_key, p_key, sizeof(prepared_key))

extern void xmxss_encrypt(uint8_t* p_data, size_t data_size, const uint8_t* p_key, size_t key_size)
{
    xor_crypt(p_data, data_size, p_key, key_size, 0, 0);
    uint8_t prepared_key[32];
    xmxss_prepare_key(p_key, key_size, prepared_key);
    struct s_multi_xorshift_seed mxs_seed;
    multi_xorshift_init(&mxs_seed, prepared_key, sizeof(prepared_key));
    multi_xorshift_shuffle(&mxs_seed, p_data, data_size);
    multi_xorshift_destruct(&mxs_seed);
}

extern void xmxss_decrypt(uint8_t* p_data, size_t data_size, const uint8_t* p_key, size_t key_size)
{
    uint8_t prepared_key[32];
    xmxss_prepare_key(p_key, key_size, prepared_key);
    struct s_multi_xorshift_seed mxs_seed;
    multi_xorshift_init(&mxs_seed, prepared_key, sizeof(prepared_key));
    multi_xorshift_deshuffle(&mxs_seed, p_data, data_size);
    multi_xorshift_destruct(&mxs_seed);
    xor_crypt(p_data, data_size, p_key, key_size, 0, 0);
}