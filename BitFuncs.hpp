#ifndef BIT_FUNCTIONS_HPP
#define BIT_FUNCTIONS_HPP

#include <cstdint>

//manipulate bit at location 'n'

//makes it a 0 at n
inline uint32_t zeroBit(uint32_t in, int n)
{
    return in & (0 << n);//zero
}

//makes it a 1 at n
inline uint32_t setBit(uint32_t in, int n)
{
    return in | (1 << n);
}

inline uint32_t setBit(uint32_t in, int n, bool b)
{
    if(b) return setBit(in, n);
    return in;//zeroBit(in, n);
}

//modified from https://www.geeksforgeeks.org/reverse-actual-bits-given-number/
inline uint32_t reverseBits(uint32_t in, unsigned int width)
{
    uint32_t reversed = 0;
    while (width)
    {
        reversed = reversed << 1;
        if(in & 1)//check last bit
        {
            reversed |= 1;
        }
        in = in >> 1;
        --width;
    }
    return reversed;
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