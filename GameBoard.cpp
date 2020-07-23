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
    //unsigned int width = CM.width;
    unsigned int height = CM.height;
    m_data.resize(CM.sz(), BACKGROUND_ID);

    m_winzoneMap.resize(height, 0);
    setWinShape(level);

    {//Function randomly places blocks without overlap
    int numTriesToPlace = 0;
    for(unsigned int i = 0; i < 12; i++)
    {
        //TODO fix for rotations and textures
        int r = rand()%4;//0
        bool isFlipped = rand()%2;//true;//false;//rand()%2;
        Pentamino block(i, Coord(0,0), rots[r], isFlipped);
        numTriesToPlace += positionBlockInFreeSpot(block);
        m_blocks.push_back(block);

        placeBlock(block);
    }
    //std::cout<<"Placing blocks took "<<numTriesToPlace<<" tries."<<std::endl;
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
        for(auto &C_ : block.m_coords)
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
    block.m_pos = Coord(x,y);
    return numTriesToPlace;
}

void GameBoard::setWinShape(const Level &level)
{
    unsigned int width = CM.width;
    unsigned int height = CM.height;

    int x_offset = (width - level.width)/2;
    int y_offset = (height - level.height)/3;
    //std::cout<<"x_ofsset is "<<x_offset<<" width "<<level.width<<std::endl;
    
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

int GameBoard::floodFill(Coord C, std::vector<bool> &visited)//bool *visited)
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
}

//see if is 'won' here
//if is in winzone, set a flag?
void GameBoard::placeBlock(Pentamino &block)
{
    Coord P = block.m_pos;

    for(unsigned int i = 0; i < 5; i++)
    {
        Coord C = P + block.m_coords[i];
        if(!CM.isValid(C)) throw;
        m_data[CM.mapID(C)] = (block.id + (i << 4));
    }
}

void GameBoard::pickupBlock(Pentamino &block)
{
    Coord P = block.m_pos;
    for(int i = 0; i < 5; ++i)
    {
        Coord C = P + block.m_coords[i];
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
//convert to a vector???
bool GameBoard::checkValidity()
{
    //bool visited[CM.sz()];//, false];
    std::vector<bool> visited(CM.sz(), false);
    //for(auto &b : visited) b = false;
    //for(uint i = 0; i < CM.sz(); ++i) v[i] = false;

    for(auto C : m_winShapeCoords)
    {
        int ctr = floodFill(C, visited);
        if(ctr % 5 != 0)
        {
            //delete[] visited;
            return false;
        }
    }
//    delete[] visited;
    return true;
}

bool GameBoard::collides(Pentamino &block)
{
    //is it colliding with another block
    //is it colliding with out of bounds
    
    for(auto &_C : block.m_coords)
    {
        Coord C = _C + block.m_pos;

        if(!CM.isValid(C))
        {
            return true;
        }
        int id = m_data[CM.mapID(C)]&0x0F;
        if(id < 12)
        {
            return true;
        }
    }
    return false;
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
            resetPiece();
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
void Controller::resetPiece()
{
    saveCoords();
    if(p_activeBlock == nullptr) return;
    //set orientation is not fixed rightly... has a bug
    p_activeBlock->setOrientation(lastOrientation, lastFlipped);
    p_activeBlock->m_pos = lastPos;
    board.placeBlock(*p_activeBlock);
    
//Hmmmmmmm hacky but redraws the piece as if was hovered so thats fine
    hoverChanged = true;
    idHover = p_activeBlock->id;

    p_activeBlock = nullptr;
    blockWasPlaced = true;//for checking winzone validity
}

void Controller::movePiece()
{
    if(p_activeBlock == nullptr) return;
    if(p_activeBlock->m_pos != m_activeCoord)
    {
        saveCoords();
        p_activeBlock->m_pos = m_activeCoord;
        blockWasMoved = true;
    }
}

//redraw the piece....
void Controller::pickUpOrPlacePiece()
{
    //placing piece
    if(p_activeBlock && !amColliding)
    {
        saveCoords();
        board.placeBlock(*p_activeBlock);
        //checkValidity();
        idHover = p_activeBlock->id;
        hoverChanged = true;
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
        lastPos = p_activeBlock->m_pos;
        lastFlipped = p_activeBlock->amFlipped;
        lastOrientation = (Orientation)p_activeBlock->m_orientation;
    }
}

void Controller::update()
{    
    //test for collisions
    if(p_activeBlock && blockWasMoved)
    {
        amColliding = (board.collides(*p_activeBlock));
    }

    //test for win condition -> all blocks are in winzone
    won = board.won();
    
    m_updatedCoords.clear();

    //winzone coloring if invalid
    if(blockWasPlaced || blockWasPickedUp)
    {
        winZoneIsValid = board.checkValidity();
        for(auto & C : board.m_winShapeCoords) m_updatedCoords.push_back(C);
    }

    if(blockWasMoved || blockWasPickedUp)
    {
        for(auto &C : lastCoords) m_updatedCoords.push_back(C);
    }

    //draw the selected piece underlaying board (for proper transparency effects)
    if(p_activeBlock)
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_activeBlock->m_coords[i] + p_activeBlock->m_pos;
            m_updatedCoords.push_back(C);
        }
    }

    //draw hover effects
    if(hoverChanged)
    {
        if(idLastHovered < 12)
        {
            Pentamino &block = board.m_blocks[idLastHovered];
            Coord P = block.m_pos;
            for(auto &C : block.m_coords) m_updatedCoords.push_back(P+C);
        }
        if(!p_activeBlock && idHover < 12)
        {
            Pentamino &block = board.m_blocks[idHover];
            Coord P = block.m_pos;
            for(auto &C : block.m_coords) m_updatedCoords.push_back(P+C);
        }
    }

    idLastHovered = idHover;

    blockWasPlaced = false;
    blockWasMoved = false;
    blockWasPickedUp = false;
}


void Controller::saveCoords()
{
    if(!p_activeBlock) return;

    Coord P = p_activeBlock->m_pos;
    for(unsigned int i = 0; i < 5; ++i)
    {
        lastCoords[i] = P + p_activeBlock->m_coords[i];
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
     hoverSatEffect(0.3f), hoverLightEffect(0.2f)
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
    }
}

void Controller::draw(DrawSettings &settings)
{
    //std::cout<<m_updatedCoords.size()<<std::endl;
    for(const Coord &C : m_updatedCoords)
    {
        settings.draw(C, grid, board, idHover, winZoneIsValid);
    }

    //draw block overlay effect
    if(p_activeBlock)
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_activeBlock->m_coords[i] + p_activeBlock->m_pos;
            if(amColliding) grid.setCellColor(C, sf::Color(255,0,0,128), true);
            else grid.setCellColor(C, sf::Color(0,255,0,128), true);

            //texture is of the block...
            TexAtlasID texId = TexAtlasID(p_activeBlock->m_texIDs[i]);
            int orientation = p_activeBlock->m_texOrientations[i];
            bool flipped = p_activeBlock->amFlipped;
            grid.setCellTexture(C, getTextureUV(texId), getTextureSize(), orientation, flipped);
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
        if(id == BACKGROUND_ID) grid.setCellTexture(C, getTextureUV(TexAtlasID::BACKGROUND_TEXTURE), getTextureSize());
        else if(id == WINZONE_ID) grid.setCellTexture(C, getTextureUV(TexAtlasID::WINZONE_TEXTURE), getTextureSize());
        else throw;
        return;
    }
    
    //a piece is on the board at this location
    int i = (id&0xF0)>>4;
    //assert(i < 5);
    id = id&0x0F;
    
    TexAtlasID texId = TexAtlasID(board.m_blocks[id].m_texIDs[i]);
    int orientation = board.m_blocks[id].m_texOrientations[i];
    bool flipped = board.m_blocks[id].amFlipped;

    grid.setCellTexture(C, getTextureUV(texId), getTextureSize(), orientation, flipped);
    
    //color of the piece at the location
    COLOR::HSL hsl;
    hsl.hue = pieceHues[id];
    hsl.lightness = baseLightness;

    if(inWinShape)
    {
        hsl.saturation = winZoneSatEffect;
    }
    else hsl.saturation = baseSaturation;

    if(board.won())
    {
        hsl.saturation = 0.9f;
        hsl.lightness = 0.6f;
    }

    if(id == idHover)
    {
        hsl.saturation += hoverSatEffect;
        hsl.lightness += hoverLightEffect;
    }

    color = COLOR::fromHSL(hsl);
    color.a = pieceAlphaTransparency;
    
    grid.setCellColor(C, color, true);
}