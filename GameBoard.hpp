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
//0xF0 mask for location in shape...
struct CellData
{
    uint8_t data;
};

//uses the grid, but is not the grid
//load the pieces and the level ... ???
//is the World... LOGIC more so than the grid. -> hmmmmmmmmm
//where can the pieces go?
//where cant the pieces go?
//Get the state of all of the pieces... etc...
//dont even need the grid, - externalize draw would be nice
//but to draw it, what do we need to do??? pass out a vector of updated coords

//just change our draw function is all... need to be a gameboard to do that
//inheritance! then need to cast and stuff
//function pointers seems better
class GameBoard
{
public:
    GameBoard();
    ~GameBoard();
    //void run();
    void reset(uint w, uint h, int level);//-1 for a playground
    void update();
    void winUpdate();//call every time an event???
    void handleEvent(const sf::Event &event);//no idea about spatial positioning, why?

    const std::vector<Pentamino> &getBlocks(){return m_blocks;}
    const std::vector<Coord> &getChangedCoords(){return m_updatedCoords;}
    const std::vector<uint8_t> &getData(){return m_data;}
    const std::vector<float> &getBlockHues(){return m_blockHues;}
    const unsigned int getHoverId(){return idHover;}
    const int blockAlpha(){return m_blockAlpha;}
    //getSelectedPieceCoords()
    Pentamino *getSelectedBlock(){return p_selectedBlock;}

    void rotatePieceRight();
    void rotatePieceLeft();
    void flipPiece();
    void movePiece(Coord C);
    void pickUpOrPlacePiece();
    void resetPiece();
    bool isWon(){return won;}
    bool isValid(){return isAValidPlacement;}//for drawing the winzone color -sigh-
    bool isColliding(){return amColliding;}
    bool isInWinShape(Coord C);
    bool piecePickedUpOrMoved(){return blockWasPickedUp || blockWasMoved;}

    CoordMapper CM;

private:
    int width, height;
//    Grid &grid;

    Coord mouseCoord;

////ALL THIS BELOW IS GAMEBOARD_SPECIFIC
    std::vector<uint8_t> m_data;
    std::vector<uint32_t> m_winzoneMap;//store in cell data...

    Pentamino *p_selectedBlock;
    bool amColliding;
    std::vector<Pentamino> m_blocks;
    std::vector<Pentamino> m_startingBlocks;
    std::vector<float> m_blockHues;//par ot the block?
    std::vector<Coord> m_updatedCoords;
    Coord lastCoords[5];
    Coord lastPos;
    bool blockWasMoved;
    bool blockWasPlaced;
    bool blockWasPickedUp;
    bool hoverChanged;
    int idLastHovered;
    int m_blockAlpha;
    
    int positionBlockInFreeSpot(Pentamino &block);
    void placeBlock(Pentamino &block);
    void pickupBlock(Pentamino &block);
    void saveCoords();
    
    unsigned int idHover;

    bool won;

    bool isAValidPlacement;
    
    void setWinShape();
    void setWinShape(int lvl);
    void checkValidity();
    int floodFill(Coord C, bool *);
    //win is if all pentaminos are in win shape.

    std::vector<Coord> m_winShapeCoords;

    //debug draw
    //std::vector<sf::Text> idTexts;

//    void draw(Coord C);//THIS CHANGES
//    void drawBlock(Pentamino &block);

    bool isPlayground;//hack
};

#endif//GAMEBOARD_HPP