#ifndef PENTAMINO_LEVEL_HPP
#define PENTAMINO_LEVEL_HPP

#include <string>
#include <vector>
#include <fstream>

//128 bytes at a time
struct LevelData
{
    uint8_t nameLength;
    char name[45];
    uint8_t numPieces;
    uint8_t ids[15];
    uint8_t width, height;
    uint32_t data[16];
};
//45 chars wide //can store as utf 8...
//can fit 32 levels in a file, set all to -1 at the beginning! //fill with FF
//fill 128 bytes for file size 4096 just because it is cool and I am bored and don't want to finish this game
struct FileInfoHeader//62 bytes for the level info
{//load the vector into memory
//stays in memory
//rewrite it out (if modified) again, use std::swap and shit
//so don't worry . let the computer worry
    uint8_t numLevels;//up to 256 levels in a file
};//4096

//am I modified???
//is an option... or save it separately
struct FileInfoFooter
{
    char hash[32];
};

//optionally may be a set of 'pieces' too
struct Level
{
    Level();
    Level(const std::string &s, unsigned int width, unsigned int height, std::vector<uint32_t> data);
    Level(unsigned int id);
    Level(const LevelData &L);
    std::string name;
    unsigned int width, height;
    std::vector<uint32_t> data;//unconstified!!!
//private:
    static const std::vector<Level> m_preLoadedLevels;
    static std::vector<Level> m_levels;
};

std::vector<Level> loadLevels(const std::string &fileName);//to do test rvo??? return value optimization
void saveLevels(const std::vector<Level> &levels);
void saveLevel(std::ofstream &file, const Level &level);
Level readLevel(std::ifstream &file);

#endif //PENTAMINO_LEVEL_HPP