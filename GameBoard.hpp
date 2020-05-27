#ifndef GAMEBOARD_HPP
#define GAMEBOARD_HPP

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
class GameBoard// : public Grid
{
public:
    GameBoard(sf::Texture*, sf::Font &font, int lvl);
    void run();
    void update();
    void render();
    void handleEvents();
private:
    sf::RenderWindow window;
    Grid grid;//drawing grid... we use...
    Coord mouseCoord;
    CoordMapper CM;

    sf::Font font;
    sf::Text text;
////ALL THIS BELOW IS GAMEBOARD_SPECIFIC
    std::vector<uint8_t> m_data;
    std::vector<uint32_t> m_winzoneMap;//store in cell data...

    Pentamino *p_selectedBlock;
    bool isColliding;
    std::vector<Pentamino> m_blocks;
    std::vector<Pentamino> m_startingBlocks;
    std::vector<float> m_blockHues;
    Coord lastCoords[5];
    bool blockWasMoved;
    bool blockWasPlaced;
    bool blockWasPickedUp;
    bool hoverChanged;
    int idLastHovered;
    int m_blockAlpha;
    
    void placeBlock(Pentamino &block);
    void pickupBlock(Pentamino &block);
    void saveCoords();

    void draw(Coord C);
    void drawBlock(Pentamino &block);
    uint32_t m_randSeed;//for the random background effect look... (draw(C) functin...)

    //bool isColliding;
    //bool inWinShape;
    //int idCollide;
    unsigned int idHover;

    bool won;

    bool isAValidPlacement;
    bool isInWinShape(Coord C);
    void setWinShape();
    void setWinShape(int lvl);
    void checkValidity();
    int floodFill(Coord C, bool *);
    //win is if all pentaminos are in win shape.

    
    //context shit up top
    
    
    
    //void clearCoord(Coord C);
    //void updateBoard();
    
    //void draw(Pentamino &block);
    

    
    //bool isValidPlace;
    std::vector<Coord> m_winShapeCoords;

    //debug draw
    std::vector<sf::Text> idTexts;
};

#endif//GAMEBOARD_HPP