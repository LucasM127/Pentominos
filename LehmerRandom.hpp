#ifndef LEHMER_RANDOM_HPP
#define LEHMER_RANDOM_HPP

#include <cstdint>

//generate random number with a seed...
//from int x and int y
//from OLC ENGINE VIDEO....
//more want the FUNCTION

//srand()
uint32_t lrand();
void lsrand(uint32_t seed);
/*
class LehmerRandomGenerator
{
public:
    void seed(uint32_t s) {m_seed = s;}
    int getRandInt(int min, int max);
    uint32_t rand();
private:
    uint32_t m_seed;
};*/

#endif