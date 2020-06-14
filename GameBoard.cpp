//TODOS:::
//Place block/pick up block algorithm... simplifiy the code!
//save / load winshapes....
//save starting positions/orientations... ... reset() the map
//hovered event?  animation? someway to verify can pick it up.... (aka brighten)
//shoot out a piece... if intersects with winzone... shoot out to not intersect... (find free spot -> local???)

//Winstate, Playground (level creation) states... act on a gameboard also
//gameboard is NOT a state...
//is an interface to the board, constraints for the grid to act on.
//PASS IT THE GRID AND SET OF PIECES

//pick up piece, put down piece, reset pieces
//IS THAT OK?  Or should i just fudge it

//BUG FOUND: put a piece back after we rotate it!  it overrides, need to rotate back to original rotation!, put in original spots 

#include "GameBoard.hpp"
#include "Colors.hpp"

#include <cmath>
#include <iostream>
#include <cassert>

#include <bitset>

//WHY THIS EXISTS IS STUPID
/*
void GameBoard::exit()
{
    idHover = 0;
    hoverChanged = false;
}
*/
//AND THIS TOO... 
void GameBoard::winUpdate()
{
    idLastHovered = idHover;
    idHover = (idHover+1)%12;
    //render the pieces
    for(auto &C_ : m_blocks[idHover].coords)
    {
        Coord C = m_blocks[idHover].defaultPos + C_;
        m_updatedCoords.push_back(C);
    }
    for(auto &C_ : m_blocks[idLastHovered].coords)
    {
        Coord C = m_blocks[idLastHovered].defaultPos + C_;
        m_updatedCoords.push_back(C);
    }
    //drawBlock(m_blocks[idHover]);
    //drawBlock(m_blocks[idLastHovered]);
}

GameBoard::~GameBoard()
{
    //std::cout<<"Ended play state"<<std::endl;
}

//need the COord, but that is all
//resize the grid? what do we do?
//resize the board
GameBoard::GameBoard()
{
}

void GameBoard::reset(uint w, uint h, int lvl)
{
    m_winzoneMap.clear();
    m_winShapeCoords.clear();
    m_blockHues.clear();
    m_data.clear();
    m_blocks.clear();

    isAValidPlacement = true;

    CM.width = w;
    CM.height = h;
    width = w;
    height = h;
    m_data.resize(CM.sz(), BACKGROUND_ID);

    srand(time(NULL));
    //set Hues
    {
        int hueRandomVariation = rand()%30;
        m_blockHues.resize(12);
        for(int i = 0; i < 12; i++)
        {
            float h = i * 30 + hueRandomVariation;
            if(h > 360.f) h -= 360.f;
            m_blockHues[i] = h / 360.f;
        }
        m_blockAlpha = 160;//192;
    }

    //selected block values...
    {
        p_selectedBlock = nullptr;
        amColliding = false;
        blockWasMoved = false;
        idHover = INVALID_ID;
        idLastHovered = INVALID_ID;
        hoverChanged = false;
        blockWasPickedUp = false;
        blockWasPlaced = false;
        won = false;
    }

    m_winzoneMap.resize(height, 0);
    setWinShape(lvl);

    {//Function randomly places blocks without overlap
    int numTriesToPlace = 0;
    for(int i = 0; i < 12; i++)
    {
        int r = 0;// rand()%4;
        bool isFlipped = false;//rand()%2;
        Pentamino block(i, Coord(0,0), rots[r], isFlipped);
        //try and find a free spot to place it...
        numTriesToPlace += positionBlockInFreeSpot(block);
        //startBlocks.push_back(block);
        m_blocks.push_back(block);

        placeBlock(block);
    }
    std::cout<<"Placing blocks took "<<numTriesToPlace<<" tries."<<std::endl;
    m_startingBlocks = m_blocks;//for resetting the map
    }

    //grid shit here
/*
    idTexts.resize(CM.sz());
    //60 sz blocks.  =. get address of coordinate?
    float cellSz = grid.getCellSize();
    for(int i = 0; i < CM.width; i++)
        for(int j = 0; j<CM.height; j++)
    {
        sf::Vector2f pos(cellSz * (float)i + 10.f, cellSz * (float)j + 10.f);
        idTexts[j * width + i].setFont(font);//at(i,j,w)
        idTexts[j * width + i].setFillColor(sf::Color::Red);
        idTexts[j * width + i].setCharacterSize(20.f);
        idTexts[j * width + i].setPosition(pos);
    }

    //draw the entire board once to initialize it...
    for(int i =0; i< width; i++)
        for(int j= 0; j< height; j++)
            draw(Coord(i,j));
*/
}

int GameBoard::positionBlockInFreeSpot(Pentamino &block)
{
    int x,y;
    int numTriesToPlace = 0;

    //find free spot
    while(true)
    {
        numTriesToPlace++;
        x = rand()%width;
        y = rand()%height;
        int ctr = 0;
        for(auto &C_ : block.coords)
        {
            Coord C = C_ + Coord(x,y);
            if(!CM.isValid(C)) break;
            int id = m_data[CM.mapID(C)];
            
            if(id != BACKGROUND_ID)
            {
                break;
            }

            ctr++;
        }
        if(ctr == 5) break;
    }
    block.defaultPos = Coord(x,y);
    return numTriesToPlace;
}

void GameBoard::rotatePieceRight()
{
    if(p_selectedBlock) 
    {
        saveCoords();
        p_selectedBlock->rotateRight();
    }
}

void GameBoard::rotatePieceLeft()
{
    if(p_selectedBlock)
    {
        saveCoords();
        p_selectedBlock->rotateLeft();
    }
}
void GameBoard::flipPiece()
{
    if(p_selectedBlock)
    {
        saveCoords();
        p_selectedBlock->flip();//clear the screen here too!
    }
}

void GameBoard::movePiece(Coord C)
{
    if(p_selectedBlock)
    {
        if(p_selectedBlock->defaultPos != C)
        {
            saveCoords();
            p_selectedBlock->defaultPos = C;
        }
    }
    else
    {//can this crash? -> yes it can
        if(CM.isValid(C))
        {
            unsigned int id = m_data[CM.mapID(C)]&0x0F;
            hoverChanged = (id != idHover);
            idLastHovered = idHover;
            idHover = id; 
        }
        else idHover = INVALID_ID;
    }
    mouseCoord = C;
}

void GameBoard::pickUpOrPlacePiece()
{
    if(CM.isValid(mouseCoord))
    {
        int id = m_data[CM.mapID(mouseCoord)]&0x0F;
        if(p_selectedBlock == nullptr && id < 12)
        {
            p_selectedBlock = &m_blocks[id];

            //pick up, set cell id to bg id, flag cells for refresh
            pickupBlock(*p_selectedBlock);

            checkValidity();//of board where we picked it up...
        }
        else if(p_selectedBlock && !amColliding)//place it
        {//check adjacent floodfill algorithm!
            saveCoords();
            placeBlock(*p_selectedBlock);
            checkValidity();
            p_selectedBlock = nullptr;
        }
    }
}

void GameBoard::resetPiece()
{
    if(p_selectedBlock != nullptr)
    {
        saveCoords();
        p_selectedBlock->defaultPos = lastPos;
//      positionBlockInFreeSpot(*p_selectedBlock);
        placeBlock(*p_selectedBlock);
        p_selectedBlock = nullptr;
    }
}

void GameBoard::update()
{    
    //test for collisions
    if(p_selectedBlock && blockWasMoved)
    {
        amColliding = false;
        int winctr = 0;
        for(auto &_C : p_selectedBlock->coords)
        {
            Coord C = _C + p_selectedBlock->defaultPos;

            if( !CM.isValid(C))
            {
                amColliding = true;
                break;
            }
            int id = m_data[CM.mapID(C)]&0x0F;
            if(id < 12)
            {
                amColliding = true;
                break;
            }
            else if(id == WINZONE_ID) winctr++;
        }
        //inWinShape = winctr == 5;
    }

    //test Win... only after a block was placed!
    //all blocks are in the winshape
    //none are 'colliding' or hovering, ... p_selectedBlock == false
    won = true;
    if(p_selectedBlock) won = false;
    //else if(blockWasPlaced)
    else
    {
        for(auto &block : m_blocks)
        {
            for(auto &C : block.coords)
            {
                if(!isInWinShape(C + block.defaultPos))
                {
                    won = false;
                    break;
                }
            }
        }
    }
//    else won = false;
/*
    //UPDATE THE DEBUG TEXT
    for(int i = 0; i < width; i++)
        for(int j = 0; j< height; j++)
    {
        sf::Vector2f pos(60.f * (float)i, 60.f * (float)j);
        Coord C = grid.getCoordinate(pos);
        idTexts[j * width + i].setString(std::to_string((m_data[CM.mapID(C)]&0x0F)));//again at(i,j,w) ????
    }
    */
    
    //render update logic copypasta
    //basically -> all the coords we need to redraw!

    //NEW WAY
    m_updatedCoords.clear();
    
    if(blockWasPlaced || blockWasPickedUp)
    {
        for(auto & C : m_winShapeCoords) m_updatedCoords.push_back(C);//draw(C);
    }

    if(blockWasMoved || blockWasPickedUp)
    {
        for(auto &C : lastCoords) m_updatedCoords.push_back(C);//draw(C);
    }

    //draw the selected piece overlay
    if(p_selectedBlock)
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_selectedBlock->coords[i] + p_selectedBlock->defaultPos;
            m_updatedCoords.push_back(C);
        }
    }
    /*
    if(p_selectedBlock && (blockWasPickedUp || blockWasMoved))
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_selectedBlock->coords[i] + p_selectedBlock->defaultPos;
            if(isColliding) grid.setCellColor(C, sf::Color(255,0,0,128), true);
            else grid.setCellColor(C, sf::Color(0,255,0,128), true);

            //texture is of the block...
            int texId = p_selectedBlock->texIDs[i];
            int orientation = p_selectedBlock->orientations[i];
            int x_off = texId%4;
            int y_off = texId/4;
            bool flipped = p_selectedBlock->isFlipped;
            sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
            grid.setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);
        }
    }
    */

    //idHover == id of where the 'mouse' is
    //Update so can draw hovered block brighter
    if(!p_selectedBlock && idHover < 12)//!= INVALID_ID)
    {
        Coord P = m_blocks[idHover].defaultPos;
        for(auto &C : m_blocks[idHover].coords) m_updatedCoords.push_back(P+C);//(P+C);
    }
    //update so previously hovered block can be drawn dimmer
    if(hoverChanged && idLastHovered < 12) //drawBlock(m_blocks[idLastHovered]);
    {
        Coord P = m_blocks[idLastHovered].defaultPos;
        for(auto &C : m_blocks[idLastHovered].coords) m_updatedCoords.push_back(P+C);//draw(P+C);
    }
    
    blockWasPlaced = false;
    blockWasMoved = false;
    blockWasPickedUp = false;
}

//assume is valid COORD
void GameBoard::placeBlock(Pentamino &block)
{
    Coord P = block.defaultPos;

    for(int i = 0; i < 5; i++)
    {
        Coord C = P + block.coords[i];
        m_data[CM.mapID(C)] = (block.id + (i << 4));
    }

    blockWasPlaced = true;
}

//set as default all the old places...
void GameBoard::pickupBlock(Pentamino &block)
{
    Coord P = block.defaultPos;
    lastPos = P;
    
    for(int i = 0; i < 5; ++i)
    {
        Coord C = P + block.coords[i];
        if(isInWinShape(C))
        {
            m_data[CM.mapID(C)] = WINZONE_ID;
        }
        else
        {
            m_data[CM.mapID(C)] = BACKGROUND_ID;
        }
    }

    blockWasPickedUp = true;
}

void GameBoard::saveCoords()
{
    if(!p_selectedBlock) return;

    Coord P = p_selectedBlock->defaultPos;
    for(int i = 0; i < 5; i++)
    {
        lastCoords[i] = P + p_selectedBlock->coords[i];
    }
    blockWasMoved = true;
}

void GameBoard::setWinShape(int lvl)
{
    //load the level
    if(lvl == -1) return;
    Level L(lvl);
    
    int x_offset = (width - L.width)/2;
    int y_offset = (height - L.height)/4 + 1;
    std::cout<<"x_ofsset is "<<x_offset<<" width "<<L.width<<std::endl;

    for(unsigned int i = 0; i < L.height; i++)
    {
        uint32_t data = reverseBits(L.data[i], L.width);
        m_winzoneMap[i + y_offset] = data << x_offset;
    }

    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
        {
            if(checkBit(m_winzoneMap[j],i))
            {
                if(CM.isValid({i,j}))
                    m_data[CM.mapID({i,j})] = WINZONE_ID;
                m_winShapeCoords.push_back(Coord(i,j));
            }
        }

    return;
}

bool GameBoard::isInWinShape(Coord C)
{
    return checkBit(m_winzoneMap[C.j], C.i);
}

int GameBoard::floodFill(Coord C, bool *visited)
{
    if(!CM.isValid(C)) return 0;
    if(visited[CM.mapID(C)]) return 0;
    int id = m_data[CM.mapID(C)];
    if(id != WINZONE_ID) return 0;
    visited[CM.mapID(C)] = true;

    int ctr = 1;

    Coord up(C.i, C.j-1);
    Coord down(C.i, C.j+1);
    Coord left(C.i-1, C.j);
    Coord right(C.i+1, C.j);
    ctr += floodFill(up,visited);
    ctr += floodFill(down,visited);
    ctr += floodFill(left,visited);
    ctr += floodFill(right,visited);

    return ctr;
};

//hmmm or just ... no id?
void GameBoard::checkValidity()
{
    //a piece has been placed or picked up
    //floodfill around...
    //create a visited array?
    bool visited[CM.sz()];
    for(auto &b : visited) b = false;
    //int areaSizes[8];

    for(auto C : m_winShapeCoords)
    {
        int ctr = floodFill(C, visited);
        if(ctr % 5 != 0)
        {
            isAValidPlacement = false;
            return;
        }
    }
    isAValidPlacement = true;
    return;
}

/*
void GameBoard::draw(Coord C)
{
    //m_impl->draw(C); ?
    if(!CM.isValid(C)) return;

    unsigned int id = m_data[CM.mapID(C)];
    bool inWinShape = isInWinShape(C);

    sf::Color color;

    if(inWinShape)
    {
        if(isAValidPlacement) color = sf::Color::White;
        else color = sf::Color(255,170,170);
    }
    else color = sf::Color::Black;
    grid.setCellColor(C, color, false);

    if( (id&0x0F) > 12)
    {
        if(id == BACKGROUND_ID) grid.setCellTexture(C, {256.f,128.f}, {128.f, 128.f});
        else if(id == WINZONE_ID) grid.setCellTexture(C, {384.f,0.f}, {128.f, 128.f});
        else throw;
        return;
    }
    
    //a piece is on the board at this location
    int i = (id&0xF0)>>4;//16 ???
    //assert(i < 5);
    id = id&0x0F;
    
    int texId = m_blocks[id].texIDs[i];
    int orientation = m_blocks[id].orientations[i];
    bool flipped = m_blocks[id].isFlipped;

    int x_off = texId%4;
    int y_off = texId/4;
    sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
    grid.setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);

    //color of the piece at the location
    HSL hsl;
    hsl.hue = m_blockHues[id];
    hsl.lightness = 0.4f;
    //float luminance = 0.4f;

    if(inWinShape)
    {
        hsl.saturation = 0.7f;
    }
    else hsl.saturation = 0.1f;

    //is a playground ?

    if(won || isPlayground)
    {
        hsl.saturation = 0.9f;// *= 1.35f;
        hsl.lightness = 0.6f;//1.f;
        //luminance = 0.6f;
    }

    if(id == idHover && p_selectedBlock == nullptr)
    {
        hsl.saturation += 0.1f;
        hsl.lightness += 0.1f;
        //luminance += 0.1f;
    }

    color = fromHSL(hsl);
    color.a = m_blockAlpha;
    //color = setLuminance(color, luminance);
    grid.setCellColor(C, color, true);
}
*/

/*
void GameBoard::handleEvent(const sf::Event &event)
{
//    sf::Event event;
//    window.waitEvent(event);
    {
    
    //if(event.type == sf::Event::Closed) window.close();
    
    if(event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Space)//back to menu...
        {
            requestStateChange(MENU);
        }
        if(event.key.code == sf::Keyboard::A)
            if(p_selectedBlock) 
            {
                saveCoords();
                p_selectedBlock->rotateRight();
            }
        if(event.key.code == sf::Keyboard::D)
            if(p_selectedBlock)
            {
                saveCoords();
                p_selectedBlock->rotateLeft();
            }
        if(event.key.code == sf::Keyboard::W)
            if(p_selectedBlock)
            {
                saveCoords();
                p_selectedBlock->flip();//clear the screen here too!
            }
        //if(event.key.code == sf::Keyboard::R)
        //    reset();
        if(event.key.code == sf::Keyboard::Escape) window.close();
        if(event.key.code == sf::Keyboard::Y) {requestStatePush(WIN); won = true;}//???}
        if(event.key.code == sf::Keyboard::L) {renderTestBool = !renderTestBool;}//???}
    }
    if(event.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord = grid.getCoordinate(mousePos);
        if(p_selectedBlock)
        {
            if(p_selectedBlock->defaultPos != mouseCoord)
            {
                saveCoords();
                p_selectedBlock->defaultPos = mouseCoord;
            }
        }
        else
        {//can this crash? -> yes it can
            if(CM.isValid(mouseCoord))
            {
                unsigned int id = m_data[CM.mapID(mouseCoord)]&0x0F;
                hoverChanged = (id != idHover);
                idLastHovered = idHover;
                idHover = id; 
            }
            else idHover = INVALID_ID;
        }
    }
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.button == sf::Mouse::Left) //(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
        //get cell id and print out
        if(CM.isValid(mouseCoord))
        {
            int id = m_data[CM.mapID(mouseCoord)]&0x0F;
            if(p_selectedBlock == nullptr && id < 12)
            {
                p_selectedBlock = &m_blocks[id];

                //pick up, set cell id to bg id, flag cells for refresh
                pickupBlock(*p_selectedBlock);

                checkValidity();//of board where we picked it up...
            }
            else if(p_selectedBlock && !isColliding)//place it
            {//check adjacent floodfill algorithm!
                saveCoords();
                placeBlock(*p_selectedBlock);
                checkValidity();
                p_selectedBlock = nullptr;
            }
        }
        }
        else if(event.mouseButton.button == sf::Mouse::Right)
        {
            //drop the pice
            if(p_selectedBlock != nullptr)
            {
                saveCoords();
                p_selectedBlock->defaultPos = lastPos;
//                positionBlockInFreeSpot(*p_selectedBlock);
                placeBlock(*p_selectedBlock);
                p_selectedBlock = nullptr;
            }
        }
    }

    }
}
*/