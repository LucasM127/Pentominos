#ifndef PENTAMINO_BLOCKS_HPP
#define PENTAMINO_BLOCKS_HPP

#include "Grid.hpp"
#include "BitFuncs.hpp"

//Palette
const unsigned int EMPTY_BLOCK = 12;
const unsigned int FILLED_BLOCK = 13;

extern sf::Color blockColors[14];
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
    static uint32_t shapes[12];
    static uint8_t shapeTextures[60];
    //mapping of the shape to its textureid and orientations...
    uint8_t texIDs[5];
    uint8_t orientations[5];
    bool isFlipped;
    unsigned int id;
    Coord defaultPos;
    //Rotation defaultRot;
    Coord coords[5];
    void rotateRight();
    void rotateLeft();
    void flip();
};

#endif//PENTAMINO_BLOCKS_HPP