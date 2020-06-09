#include "LehmerRandom.hpp"

uint32_t LSeed;

void lsrand(uint32_t seed)
{
    LSeed = seed;
}

uint32_t lrand()
{
    LSeed += 0xe120fc15;
    uint64_t tmp;
    tmp = (uint64_t)LSeed * 0x4a39b70d;
    uint32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (tmp >> 32) ^ tmp;
    return m2;
}
/*
//LehmerRandomGenerator(){}
//void setSeed(int i, int j);
//void setSeed(int k);
void LehmerRandomGenerator::seed(uint32_t s)
{
    m_seed = s;
}

//copy pasta
uint32_t LehmerRandomGenerator::rand()
{
    m_seed += 0xe120fc15;
    uint64_t tmp;
    tmp = (uint64_t)m_seed * 0x4a39b70d;
    uint32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (tmp >> 32) ^ tmp;
    return m2;
}*/