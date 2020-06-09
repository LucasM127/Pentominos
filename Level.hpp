#ifndef PENTAMINO_LEVEL_HPP
#define PENTAMINO_LEVEL_HPP

#include <string>
#include <vector>

//just a bunch of numbers!
struct Level
{
    Level();
    Level(const std::string &s, unsigned int width, unsigned int height, std::vector<uint32_t> data);
    Level(unsigned int id);
    const std::string name;
    const unsigned int width, height;
    const std::vector<uint32_t> data;
//private:
    static const std::vector<Level> m_preLoadedLevels;
};

#endif //PENTAMINO_LEVEL_HPP