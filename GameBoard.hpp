#ifndef GAMEBOARD_HPP
#define GAMEBOARD_HPP

//States of gameboard ???
//or something
//common functions, different defines
//playground, -> list of pieces, (yes), winzone (no), draw pieces brighter
//common pieces here
//Playing -> list of pieces (yes), winzone (yes), draw dimmer
//Won -> list of pieces (yes) winzone (yes), no input just do fancy win effects
//Menu/Intro screen -> no list of pieces just the bg grid

//state, -> different base ingredients, and then custom crap in between

#include "Pentaminos.hpp"
#include "Level.hpp"
#include "Grid.hpp"

const unsigned int WINZONE_ID = 14;
const unsigned int BACKGROUND_ID = 15;
const unsigned int INVALID_ID = -1;

//0x0F mask for id
//0xF0 mask for location in shape..
struct CellData
{
    uint8_t data;    
};
/*
struct CellData
{
    union{
    uint8_t data;
    struct
    {
        uint8_t id : 4;
        uint8_t subId : 4;
    };
    };
};*/

struct GameBoard
{
public:
    GameBoard(){}
    ~GameBoard(){}
    void set(const CoordMapper &mapper, const Level &level);//int level);//-1 for a playground
    void setWinShape(const Level &level);//int level);
    int positionBlockInFreeSpot(Pentamino &block);
    void pickupBlock(Pentamino &block);
    void placeBlock(Pentamino &block);
    bool isInWinShape(Coord C);
    int floodFill(Coord C, bool *visited);
    bool checkValidity();
    int collides(Pentamino &block);
    bool won();
    Pentamino *getBlock(Coord C);
    uint32_t get(Coord C);

    CoordMapper CM;//width height of the board
    std::vector<uint8_t> m_data;
    std::vector<uint32_t> m_winzoneMap;
    std::vector<Pentamino> m_blocks;
    std::vector<Coord> m_winShapeCoords;
};

struct DrawSettings;

//manipulates the board// ie the Player
//constant... for the game...
//winzone effect controller??? no update events there... :/
//should be able to just draw the board ... but
//hover effects and the like - shit =
class Controller
{
public:
    Controller(GameBoard &_board, Grid &_grid);
    void handleEvent(const sf::Event &event);
    void draw(DrawSettings &S);
private:
    GameBoard &board;
    Grid &grid;
    Coord m_activeCoord;

    std::vector<Pentamino> m_startingBlocks;
    std::vector<Coord> m_updatedCoords;

    Pentamino *p_activeBlock;
    bool amColliding;
    bool won;
    bool winZoneIsValid;

    Coord lastCoords[5];
    Coord lastPos;
    bool blockWasMoved;
    bool blockWasPlaced;
    bool blockWasPickedUp;
    bool hoverChanged;
    int idLastHovered;
    int idHover;

    void rotatePieceRight();
    void rotatePieceLeft();
    void flipPiece();
    void movePiece();
    void setActive(Coord C);
    void pickUpOrPlacePiece();
    //void resetPiece();
    void saveCoords();
    void update();
};

//then each ... just use the draw function with the struct!
struct DrawSettings
{
public:
    DrawSettings();
    void draw(Coord C, Grid &grid, GameBoard &board,
                unsigned int idHover = INVALID_ID,
                bool winZoneValid = true);
    std::vector<float> pieceHues;
    int pieceAlphaTransparency;
    float baseLightness;
    float baseSaturation;
    float winZoneSatEffect;
    float winZoneLightEffect;
    float hoverSatEffect;
    float hoverLightEffect;
};
//winzone draw -> for each piece, draw as if idHover is different
//redraw the 2 pieces

#endif//GAMEBOARD_HPP