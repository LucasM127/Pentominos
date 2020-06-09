#ifndef BIT_FUNCTIONS_HPP
#define BIT_FUNCTIONS_HPP

#include <cstdint>

//manipulate bit at location 'n'

inline uint32_t setBit(uint32_t in, int n)
{
    return in | (1 << n);
}

inline bool checkBit(uint32_t s, int n)
{
    return s & 1 << n;
}

inline void toggleBit(uint32_t &in, int n)
{
    in = in ^ (1 << n);
}

#endif