#ifndef PENTAMINO_BLOCKS_HPP
#define PENTAMINO_BLOCKS_HPP

//TODO: enum -> enum class : type == uint8_t // hope this works...

#include "Grid.hpp"
#include "BitFuncs.hpp"

//Palette
const unsigned int EMPTY_BLOCK = 12;
const unsigned int FILLED_BLOCK = 13;

//extern sf::Color blockColors[14];
//sf::Vector2f blockTexCoords[14];

//exists in a 5x5 grid with origin 0,0 ?

enum Orientation : uint8_t
{
    DEFAULT = 0,
    R90CCW,
    R180,
    R90CW
};
extern const Orientation rots[4];

enum TexAtlasID : uint8_t
{
    INSIDE_PIECE = 0,
    SINGLE_WALL,
    CHANNEL_WALLS,
    WINZONE_TEXTURE,
    ANGLE_WALLS,
    END_WALLS,
    BACKGROUND_TEXTURE,
    EMPTY
};

//hmmm????
//TexAtlas.mapTexture()

inline sf::Vector2f getTextureUV(TexAtlasID texId)
{
    int x_off = texId%4;
    int y_off = texId/4;
    return sf::Vector2f(128.f * (float)x_off, 128.f * (float)y_off);
}

inline sf::Vector2f getTextureSize()
{
    return sf::Vector2f(128.f, 128.f);
}

//Wikipedias naming convention
//https://en.wikipedia.org/wiki/Pentomino#/media/File:Pentomino_Naming_Conventions.svg
enum PentaminoShape : int
{
    I = 0,
    L,
    N,
    Y,
    X,
    P,
    T,
    U,
    V,
    W,
    F,
    Z
};

struct Pentamino
{
    Pentamino(unsigned int i, Coord c, Orientation r, bool flipped);

    void rotateRight();
    void rotateLeft();
    void rotate180();
    void flip();
    void setOrientation(Orientation O, bool flipped);

    const unsigned int id;

    uint8_t m_orientation;
    bool amFlipped;
    Coord m_pos;
    Coord m_coords[5];
    //for texture mapping of the coord to texAtlas
    uint8_t m_texIDs[5];
    uint8_t m_texOrientations[5];

    static uint32_t shapes[12];
    static uint8_t shapeTextures[60];
};

#endif//PENTAMINO_BLOCKS_HPP