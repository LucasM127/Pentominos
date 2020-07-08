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

void GameBoard::set(const CoordMapper &mapper, const Level &level)//int lvl)
{
    m_winzoneMap.clear();
    m_winShapeCoords.clear();
    m_data.clear();
    m_blocks.clear();

    CM = mapper;
    unsigned int width = CM.width;
    unsigned int height = CM.height;
    m_data.resize(CM.sz(), BACKGROUND_ID);

    m_winzoneMap.resize(height, 0);
    setWinShape(level);

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
    }
}

int GameBoard::positionBlockInFreeSpot(Pentamino &block)
{
    int x,y;
    int numTriesToPlace = 0;

    //find free spot
    while(true)
    {
        numTriesToPlace++;
        x = rand()%CM.width;
        y = rand()%CM.height;
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

void GameBoard::setWinShape(const Level &level)//int lvl)
{
    //load the level
//    if(lvl == -1) return;
//    Level L(lvl);//Level::m_levels[lvl]);//(lvl);//unless I just make it global again ?
    unsigned int width = CM.width;
    unsigned int height = CM.height;

    int x_offset = (width - level.width)/2;
    int y_offset = (height - level.height)/4 + 1;
    std::cout<<"x_ofsset is "<<x_offset<<" width "<<level.width<<std::endl;

    for(unsigned int i = 0; i < level.height; i++)
    {
        uint32_t data = reverseBits(level.data[i], level.width);
        m_winzoneMap[i + y_offset] = data << x_offset;
    }

    for(unsigned int i = 0; i < width; i++)
        for(unsigned int j = 0; j < height; j++)
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

//see if is 'won' here
//if is in winzone, set a flag?
void GameBoard::placeBlock(Pentamino &block)
{
    Coord P = block.defaultPos;

    for(unsigned int i = 0; i < 5; i++)
    {
        Coord C = P + block.coords[i];
        if(!CM.isValid(C)) throw;
        m_data[CM.mapID(C)] = (block.id + (i << 4));
    }
}

void GameBoard::pickupBlock(Pentamino &block)
{
    Coord P = block.defaultPos;
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
}

Pentamino *GameBoard::getBlock(Coord C)
{
    if(CM.isValid(C))
    {
        int id = m_data[CM.mapID(C)]&0x0F;//.id;//&0x0F;
        if(id < 12)
        {
            return &m_blocks[id];
        }
    }
    return nullptr;
}

uint32_t GameBoard::get(Coord C)
{
    if(CM.isValid(C))
    {
        return m_data[CM.mapID(C)];
    }
    return 0xFFFFFFFF;
}

//is winzone area a multiple of 5?
bool GameBoard::checkValidity()
{
    bool visited[CM.sz()];
    for(auto &b : visited) b = false;

    for(auto C : m_winShapeCoords)
    {
        int ctr = floodFill(C, visited);
        if(ctr % 5 != 0)
        {
            return false;
        }
    }
    return true;
}

//int???
int GameBoard::collides(Pentamino &block)
{
    //is it colliding with another block
    //is it colliding with out of bounds
    //is it ALL in the winzone
    int winctr = 0;
    for(auto &_C : block.coords)
    {
        Coord C = _C + block.defaultPos;

        if(!CM.isValid(C))
        {
            return 2;
        }
        int id = m_data[CM.mapID(C)]&0x0F;
        if(id < 12)
        {
            return 2;
        }
        else if(id == WINZONE_ID) winctr++;
    }
    if(winctr == 5) return 1;
    return 0;
}

//need to check the winCoords to the ids on the board,
//not to each block coord
bool GameBoard::won()
{
    for(auto &C : m_winShapeCoords)
    {
        if( (m_data[CM.mapID(C)] & 0xF) >= 12) return false;
    }
    return true;
}

Controller::Controller(GameBoard &_board, Grid &_grid) : board (_board), grid(_grid), p_activeBlock(nullptr)
{
    amColliding = false;
    idHover = idLastHovered = INVALID_ID;
}

void Controller::handleEvent(const sf::Event &event)
{
    switch (event.type)
    {
    case sf::Event::KeyPressed:
        switch (event.key.code)
        {
        case sf::Keyboard::A:
            rotatePieceRight();
            break;
        case sf::Keyboard::D:
            rotatePieceLeft();
            break;
        case sf::Keyboard::W:
            flipPiece();
            break;
        default:
            break;
        }
        break;
    case sf::Event::MouseMoved:
        {
        //no 'worldview' mapping required
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        //sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        m_activeCoord = grid.getCoordinate(mousePos);
        //idHovered = board.getId(m_activeCoord);
        idHover = board.get(m_activeCoord)&0x0F;
        if(idLastHovered != idHover) hoverChanged = true;
        else hoverChanged = false;

        movePiece();
        }
        break;
    case sf::Event::MouseButtonPressed:
    {
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            pickUpOrPlacePiece();
        }
        else if(event.mouseButton.button == sf::Mouse::Right)
        {
            ;//resetPiece();
        }
    }
    default:
        break;
    }

    update();
}

void Controller::rotatePieceRight()
{
    if(p_activeBlock == nullptr) return;
    saveCoords();
    p_activeBlock->rotateRight();
}

void Controller::rotatePieceLeft()
{
    if(p_activeBlock == nullptr) return;
    saveCoords();
    p_activeBlock->rotateLeft();
}
void Controller::flipPiece()
{
    if(p_activeBlock == nullptr) return;
    saveCoords();
    p_activeBlock->flip();//clear the screen here too!
}

void Controller::movePiece()
{
    if(p_activeBlock == nullptr) return;
    if(p_activeBlock->defaultPos != m_activeCoord)
    {
        saveCoords();
        p_activeBlock->defaultPos = m_activeCoord;
        blockWasMoved = true;
    }
}

//Code to get Hover ID
/*
            unsigned int id = m_data[CM.mapID(C)]&0x0F;//.id;//&0x0F;
            hoverChanged = (id != idHover);
            idLastHovered = idHover;
            idHover = id; 
        }
        else idHover = INVALID_ID;
*/

void Controller::pickUpOrPlacePiece()
{
    if(p_activeBlock && !amColliding)
    {
        saveCoords();
        board.placeBlock(*p_activeBlock);
        //checkValidity();
        p_activeBlock = nullptr;
        blockWasPlaced = true;
    }
    else if(p_activeBlock == nullptr)
    {
        //better to get ID and block...???
        p_activeBlock = board.getBlock(m_activeCoord);
        if(p_activeBlock == nullptr) return;
        board.pickupBlock(*p_activeBlock);
        blockWasPickedUp = true;
    }
}

void Controller::update()
{    
    //basically how are we to draw the board?
    //amColliding = green/red over lay piece
    //have we changed our idHover

    //test for collisions
    if(p_activeBlock && blockWasMoved)
    {
        amColliding = (board.collides(*p_activeBlock) == 2);
    }

    //test for win condition -> all blocks are in winzone
    won = board.won();
    
    m_updatedCoords.clear();
    
    if(blockWasPlaced || blockWasPickedUp)
    {
        winZoneIsValid = board.checkValidity();
        for(auto & C : board.m_winShapeCoords) m_updatedCoords.push_back(C);//draw(C);
    }

    if(blockWasMoved || blockWasPickedUp)
    {
        for(auto &C : lastCoords) m_updatedCoords.push_back(C);//draw(C);
    }

    //draw the selected piece overlay
    if(p_activeBlock)
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_activeBlock->coords[i] + p_activeBlock->defaultPos;
            m_updatedCoords.push_back(C);
        }
    }

    //draw the blocks!?
    //idHover == id of where the 'mouse' is
    //Update so can draw hovered block brighter
    if(!p_activeBlock && idHover < 12)//!= INVALID_ID)
    {
        Pentamino &block = board.m_blocks[idHover];
        Coord P = block.defaultPos;
        for(auto &C : block.coords) m_updatedCoords.push_back(P+C);//(P+C);
    }
    //update so previously hovered block can be drawn dimmer
    if(hoverChanged && idLastHovered < 12) //drawBlock(m_blocks[idLastHovered]);
    {
        Pentamino &block = board.m_blocks[idLastHovered];
        Coord P = block.defaultPos;
        for(auto &C : block.coords) m_updatedCoords.push_back(P+C);
    }
    
    idLastHovered = idHover;//

    blockWasPlaced = false;
    blockWasMoved = false;
    blockWasPickedUp = false;
}


void Controller::saveCoords()
{
    if(!p_activeBlock) return;

    Coord P = p_activeBlock->defaultPos;
    for(int i = 0; i < 5; i++)
    {
        lastCoords[i] = P + p_activeBlock->coords[i];
    }
    blockWasMoved = true;
}

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

*/

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

DrawSettings::DrawSettings()
    :pieceAlphaTransparency(160), baseLightness(0.5f), baseSaturation(0.05f),
     winZoneSatEffect(0.7f), winZoneLightEffect(19887.f),
     hoverSatEffect(0.1f), hoverLightEffect(0.1f)
{
    {
        int hueRandomVariation = rand()%30;
        pieceHues.resize(12);
        for(int i = 0; i < 12; i++)
        {
            float h = i * 30 + hueRandomVariation;
            if(h > 360.f) h -= 360.f;
            pieceHues[i] = h / 360.f;
        }
        //alpha = 160;//192;
    }
}

void Controller::draw(DrawSettings &settings)
{
    for(const Coord &C : m_updatedCoords)
    {
        settings.draw(C, grid, board, idHover, winZoneIsValid);
    }

    //draw block overlay effect
    if(p_activeBlock)
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_activeBlock->coords[i] + p_activeBlock->defaultPos;
            if(amColliding) grid.setCellColor(C, sf::Color(255,0,0,128), true);
            else grid.setCellColor(C, sf::Color(0,255,0,128), true);

            //texture is of the block...
            int texId = p_activeBlock->texIDs[i];
            int orientation = p_activeBlock->orientations[i];
            int x_off = texId%4;
            int y_off = texId/4;
            bool flipped = p_activeBlock->isFlipped;
            sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
            grid.setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);
        }
    }
}

void DrawSettings::draw(Coord C, Grid &grid, GameBoard &board,
                unsigned int idHover,
                bool winZoneIsValid)
{
    CoordMapper &CM = board.CM;

    if(!CM.isValid(C)) return;
    unsigned int id = board.m_data[CM.mapID(C)];
    bool inWinShape = board.isInWinShape(C);

    sf::Color color;

    if(inWinShape)
    {
        if(winZoneIsValid) color = sf::Color::White;
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
    
    TexAtlasID texId = TexAtlasID(board.m_blocks[id].texIDs[i]);
    int orientation = board.m_blocks[id].orientations[i];
    bool flipped = board.m_blocks[id].isFlipped;

    grid.setCellTexture(C, getTextureUV(texId), getTextureSize(), orientation, flipped);
    
    //color of the piece at the location
    COLOR::HSL hsl;
    hsl.hue = pieceHues[id];//hues[id];
    hsl.lightness = baseLightness;//0.5f;//0.4f;
    //float luminance = 0.4f;

    if(inWinShape)
    {
        hsl.saturation = winZoneSatEffect;// 0.7f;
    }
    else hsl.saturation = baseSaturation;// 0.05f;

    if(board.won())
    {
        hsl.saturation = 0.9f;//*= 1.35f;
        hsl.lightness = 0.6f;//1.f;
    }

    if(id == idHover)
    {
        hsl.saturation += hoverSatEffect;//0.1f;
        hsl.lightness += hoverLightEffect;//0.1f;
        //luminance += 0.1f;
    }

    color = COLOR::fromHSL(hsl);
    color.a = pieceAlphaTransparency;
    //color = setLuminance(color, luminance);
    grid.setCellColor(C, color, true);
}