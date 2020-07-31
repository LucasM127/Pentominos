#ifndef GAMEBOARD_HPP
#define GAMEBOARD_HPP

#include "Pentaminos.hpp"
#include "Level.hpp"
#include "Grid.hpp"

const unsigned int WINZONE_ID = 14;
const unsigned int BACKGROUND_ID = 15;
const unsigned int INVALID_ID = -1;

//fixed playable gameboard size for level loading stuffs
const unsigned int BOARD_WIDTH = 22;
const unsigned int BOARD_HEIGHT = 14;

struct ViewRect
{
    inline void set(unsigned int px, unsigned int py, unsigned int w, unsigned int h)
    {
        P.i = px;
        P.j = py;
        width = w;
        height = h;
    }
    inline Coord toGlobal(Coord C){ return (P + C); }
    inline bool localTransform(Coord &C)
    {
        C.i -= P.i;
        C.j -= P.j;
        return (C.i < width && C.j < height);
    }
//    inline Coord toLocal(Coord C){ return Coord(C.i - P.i, C.j - P.j); }
    //inline Coord transform(Coord C){ return (P + C); }
    Coord P;
    unsigned int width, height;
};

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
    void set(const CoordMapper &mapper, const Level &level);
    void setWinShape(const Level &level);
    int positionBlockInFreeSpot(Pentamino &block);
    void pickupBlock(Pentamino &block);
    void placeBlock(Pentamino &block);
    bool isInWinShape(Coord C);
    int floodFill(Coord C, std::vector<bool> &visited);
    bool checkValidity();
    bool collides(Pentamino &block);
    bool won();
    Pentamino *getBlock(Coord C);
    uint32_t get(Coord C);

    CoordMapper CM;//width height of the board//set equal to size of the grid, but not necessarily
    std::vector<uint8_t> m_data;
    std::vector<uint32_t> m_winzoneMap;
    std::vector<Pentamino> m_blocks;
    std::vector<Coord> m_winShapeCoords;
};

struct DrawSettings;

//manipulates the board// ie the Player
class Controller
{
public:
    Controller(GameBoard &_board, Grid &_grid, ViewRect &rect, sf::Color c, TexAtlasID tid);
    void handleEvent(const sf::Event &event);
    void draw(DrawSettings &S);
    void drawAll(DrawSettings &S);
private:
    GameBoard &board;
    Grid &grid;
    Coord m_activeCoord;

    ViewRect &m_viewRect;//reference...
    sf::Color borderColor;
    TexAtlasID borderTexID;

    std::vector<Pentamino> m_startingBlocks;
    std::vector<Coord> m_updatedCoords;

    Pentamino *p_activeBlock;
    bool amColliding;
    bool won;
    bool winZoneIsValid;

    Coord lastCoords[5];
    Coord lastPos;
    Orientation lastOrientation;
    bool lastFlipped;
    bool blockWasMoved;
    bool blockWasPlaced;
    bool blockWasPickedUp;
    bool hoverChanged;
    uint idLastHovered;
    uint idHover;

    void rotatePieceRight();
    void rotatePieceLeft();
    void flipPiece();
    void movePiece();
    void setActive(Coord C);
    void pickUpOrPlacePiece();
    void resetPiece();
    void saveCoords();
    void update();
};

struct DrawSettings
{
public:
    DrawSettings();
    bool draw(Coord C, Grid &grid, GameBoard &board, ViewRect &viewRect,
                sf::Color borderColor, TexAtlasID borderTexID, unsigned int idHover = INVALID_ID, 
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

#endif//GAMEBOARD_HPP