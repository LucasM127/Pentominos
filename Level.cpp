#include "Level.hpp"

#include <cassert>
#include <sstream>
#include <cstring>

std::vector<Level> Level::m_levels;

/*
//128 bytes at a time
struct LevelData
{
    uint8_t nameLength;
    char name[45];
    uint8_t numPieces;
    uint8_t ids[15];
    uint8_t width, height;
    uint32_t data[16];
};//THis seems to be great
*/

//save a file!
void saveLevel(std::ofstream &file, const Level &level)
{
    LevelData L;
    L.nameLength = level.name.length();
    memcpy(L.name, level.name.c_str(), L.nameLength * sizeof(char));
    //L.numPieces = ;
    //L.ids[] = memcpy
    L.width = level.width;
    L.height = level.height;
    memcpy(L.data, &level.data[0], sizeof(uint32_t) * level.data.size());

    file.write((char*)&L, sizeof(LevelData));
}

Level::Level(const LevelData &L)
{
    name.reserve(L.nameLength);
    memcpy(&name[0], L.name, L.nameLength * sizeof(char));
    width = L.width;
    height = L.height;
    data.reserve(height);
    memcpy(&data[0], L.data, sizeof(uint32_t) * height);
}

std::vector<Level> loadLevels(const std::string &fileName)
{
    std::ifstream file("Assets/" + fileName);
    if(!file) throw std::runtime_error(fileName + " not found");
    FileInfoHeader FH;
    file.read((char*)&FH, sizeof(FH));
    
    std::vector<Level> levels;

    LevelData L;
    for(auto i = 0; i < FH.numLevels; ++i)
    {
        file.read((char*)&L, sizeof(LevelData));
        if(!file) throw std::runtime_error("Unable to read in level");
        levels.emplace_back(L);
    }

    return levels;
}

void saveLevels(const std::vector<Level> &levels, const std::string &fileName)
{
    std::ofstream file("Assets/" + fileName);
    if(!file) throw std::runtime_error("Unable to write to " + fileName);
    
    FileInfoHeader FH;
    FH.numLevels = levels.size();
    file.write((char*)&FH, sizeof(FH));

    for(auto &lvl : levels)
    {
        saveLevel(file, lvl);
    }
    file.close();
}

Level::Level() : width(0), height(0), data() {}

Level::Level(const std::string &s, unsigned int w, unsigned int h, std::vector<uint32_t> d)
            : name(s), width(w), height(h), data(d)
{}

Level::Level(unsigned int id) : Level(id < m_preLoadedLevels.size() ?
                            m_preLoadedLevels[id] :
                            m_preLoadedLevels[0])
{}

const std::vector<Level> Level::m_preLoadedLevels = 
{
{
    "Create Level",
    8,
    8,
    {
    0b00011000,
    0b00011000,
    0b00011000,
    0b11111111,
    0b11111111,
    0b00011000,
    0b00011000,
    0b00011000,
    }
},
{
    "Change Folders",
    10,
    10,
    {
    0b0000111111,
    0b0000000001,
    0b0011111101,
    0b0000000101,
    0b1111110101,
    0b1111110101,
    0b1111110100,
    0b1111110100,
    0b1111110000,
    0b1111110000,
    }
},
{
    "3 x 20 Rectangle",
    20,
    3,
    {
    0b11111111111111111111,
    0b11111111111111111111,
    0b11111111111111111111
    }
},
{
    "4 x 15 Rectangle",
    15,
    4,
    {
    0b111111111111111,
    0b111111111111111,
    0b111111111111111,
    0b111111111111111
    }
},
{
    "5 x 12 Rectangle",
    12,
    5,
    {
    0b111111111111,
    0b111111111111,
    0b111111111111,
    0b111111111111,
    0b111111111111
    }
},
{
    "6 x 10 Rectangle",
    10,
    6,
    {
    0b1111111111,
    0b1111111111,
    0b1111111111,
    0b1111111111,
    0b1111111111,
    0b1111111111
    }
},
{
    "8 x 8 Square\n(with 2 x 2 Hole)",
    8,
    8,
    {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11100111,
    0b11100111,
    0b11111111,
    0b11111111,
    0b11111111
    }
},
{
    "8 x 8 Square\n(with Corners Clipped)",
    8,
    8,
    {
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110
    }
},
{
    "Elephant\n(side view)",
    10,
    11,
    {
    0b0000000110,
    0b0000001111,
    0b0000011111,
    0b0000011111,
    0b0111111011,
    0b1111111001,
    0b1111111001,
    0b1111111000,
    0b1110011000,
    0b0110011000,
    0b0110011000
    }
},
{
    "Fish",
    12,
    9,
    {
    0b000000110000,
    0b100001111000,
    0b100111111100,
    0b110111111110,
    0b111111111111,
    0b110111111110,
    0b100011111100,
    0b100000111000,
    0b000000110000
    }
},
{
    "Tree",
    11,
    10,
    {
    0b00001110100,
    0b00111111110,
    0b11111111110,
    0b11110111111,
    0b01110111111,
    0b00110110111,
    0b00010110010,
    0b00000110000,
    0b00000110000,
    0b00001111000
    }
},
{
    "Elephant Face\n(front view)",
    11,
    8,
    {
    0b11111011111,
    0b11111011111,
    0b11111111111,
    0b11111111111,
    0b11111111111,
    0b00011111000,
    0b00000100000,
    0b00000100000
    }
},
{
    "C Plus",
    12,
    9,
    {
    0b111111000000,
    0b111111000000,
    0b111111000110,
    0b111000000111,
    0b111000001111,
    0b111000001110,
    0b111111001110,
    0b111111000000,
    0b111111000000
    }
},
{
    "Two Little Duckies",
    18,
    7,
    {
    0b011100000011100000,
    0b010100000010100000,
    0b111100000111110000,
    0b001101110001111011,
    0b001111110001111110,
    0b001111100000111100,
    0b001111100000111100
    }
},
{
    "Beetle\n(by Lucas)",
    12,
    8,
    {
    0b000000001110,
    0b001111111000,
    0b011111111000,
    0b111111111111,
    0b111111111110,
    0b111111111000,
    0b011010001000,
    0b110011001100
    }
},
{
    "RandoPieces",
    14,
    9,
    {
    0b01101110111001,
    0b11001010010001,
    0b10000100010001,
    0b00001100100101,
    0b00110111110101,
    0b10110000101100,
    0b10100001001001,
    0b10001001110001,
    0b11011110010111
    }
},
{
    "L Shape",
    10,
    10,
    {
    0b1111111110,
    0b1111111110,
    0b1111111110,
    0b1110000000,
    0b1110111110,
    0b1110000000,
    0b1110011000,
    0b1110010010,
    0b1110110111,
    0b0000000010
    }
}
};
/*
{
    10,
    10,
    {
    0b0000110000,
    0b0001111000,
    0b0011111100,
    0b0111111110,
    0b1111111111,
    0b1111111111,
    0b0111111110,
    0b0011111100,
    0b0001111000,
    0b0000110000
    }
},
{
    22,
    14,
    {
    0b0000000000000000000000,
    0b0111011101000101110111,
    0b0101010001100100100101,
    0b0111011101010100100101,
    0b0100010001001100100101,
    0b0100011101000100100111,
    0b0000000000000000000000,
    0b011111010100010111000,
    0b010101010110010101000,
    0b010001010101010101000,
    0b010001010100110101000,
    0b010001010100010111000,
    0b0000000000000000000000,
    0b0000000000000000000000
    }
}
};*/
