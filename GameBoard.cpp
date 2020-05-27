//TODOS:::
//Place block/pick up block algorithm... simplifiy the code!
//save / load winshapes....
//save starting positions/orientations... ... reset() the map
//hovered event?  animation? someway to verify can pick it up.... (aka brighten)
//shoot out a piece... if intersects with winzone... shoot out to not intersect... (find free spot -> local???)

//Load pieces
//each piece has a default color...
//brighten/darken/desaturate
//place pieces...
//for each ... on board... then draw the board.

#include "GameBoard.hpp"
#include "Colors.hpp"

#include <cmath>
#include <iostream>
#include <cassert>

//better to save the hue... for each piece... from color all goes red as hue is zero = red!
GameBoard::GameBoard(sf::Texture *p_texture, sf::Font &font, int lvl) : grid(22,14,60.f), p_selectedBlock(nullptr)
{
    window.create(sf::VideoMode(1320, 840), "Pentominos", sf::Style::Close);
    //create a coordinate mapper same size as the grid for our interior grid ???
    CM.width = grid.getWidth();
    CM.height = grid.getHeight();
    m_data.resize(m_width*m_height, BACKGROUND_ID);
    
    srand(time(NULL));
    m_randSeed = rand();
    setTexMap(p_texture);

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
        m_blockAlpha = 192;
    }

    //selected block values...
    {
        p_selectedBlock = nullptr;
        isColliding = false;
        blockWasMoved = false;
        idHover = INVALID_ID;
        won = false;
    }
    
    m_winzoneMap.resize(m_height, 0);
    setWinShape(lvl);
    
    {//Function randomly places blocks without overlap
    int numTriesToPlace = 0;
    for(int i = 0; i < 12; i++)
    {
        int r = 0;// rand()%4;
        bool isFlipped = false;//rand()%2;
        Pentamino block(i, Coord(0,0), rots[r], isFlipped);
        //try and find a free spot to place it...
        int x,y;

        //find free spot
        while(true)
        {
            numTriesToPlace++;
            x = rand()%m_width;
            y = rand()%m_height;
            int ctr = 0;
            for(auto &C_ : block.coords)
            {
                Coord C = C_ + Coord(x,y);
                if(!isValid(C)) break;
                int id = m_data[mapID(C)];
                
                if(id != BACKGROUND_ID)
                {
                    break;
                }

                ctr++;
            }
            if(ctr == 5) break;
        }
        block.defaultPos = Coord(x,y);
        //startBlocks.push_back(block);
        m_blocks.push_back(block);

        placeBlock(block);
    }
    std::cout<<"Placing blocks took "<<numTriesToPlace<<" tries."<<std::endl;
    m_startingBlocks = m_blocks;//for resetting the map
    }

    text.setFont(font);
    text.setFillColor(sf::Color::Green);
    text.setPosition(10.f,10.f);

    idTexts.resize(m_width * m_height);
    //60 sz blocks.  =. get address of coordinate?
    for(int i = 0; i < m_width; i++)
        for(int j = 0; j<m_height; j++)
    {
        sf::Vector2f pos(getCellSize() * (float)i + 10.f, getCellSize() * (float)j + 10.f);
        idTexts[j * m_width + i].setFont(font);
        idTexts[j * m_width + i].setFillColor(sf::Color::Red);
        idTexts[j * m_width + i].setCharacterSize(20.f);
        idTexts[j * m_width + i].setPosition(pos);
    }

    //draw the entire board once to initialize it...
    for(int i =0; i< m_width; i++)
        for(int j= 0; j<m_height; j++)
            draw(Coord(i,j));
} 

void GameBoard::update()
{    
    //the logic
    
    //test for collisions
    if(p_selectedBlock && blockWasMoved)
    {
        isColliding = false;
        int winctr = 0;
        for(auto &_C : p_selectedBlock->coords)
        {
            Coord C = _C + p_selectedBlock->defaultPos;

            if( !isValid(C))
            {
                isColliding = true;
                break;
            }
            int id = m_data[mapID(C)]&0x0F;
            if(id < 12)
            {
                isColliding = true;
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

    if(!won) text.setString("Keep Trying...");
    else 
    {
        //srand(time(NULL));
        //idHover = rand()%12;
        text.setString("YAY!");
        //request state change back...
    }


    for(int i = 0; i < getWidth(); i++)
        for(int j = 0; j< getHeight(); j++)
    {
        sf::Vector2f pos(60.f * (float)i, 60.f * (float)j);
        Coord C = getCoordinate(pos);
        idTexts[j * getWidth() + i].setString(std::to_string((m_data[mapID(C)]&0x0F)));
    }
    
}

void GameBoard::drawBlock(Pentamino &block)
{
    Coord P = block.defaultPos;
    for(auto &C : block.coords) draw(P+C);
}

void GameBoard::render()
{
    /*for(int i =0; i< m_width; i++)
        for(int j= 0; j<m_height; j++)
            draw(Coord(i,j));*/
    if(blockWasPlaced || blockWasPickedUp)
    {
        for(auto & C : m_winShapeCoords) draw(C);
    }

    if(blockWasMoved || blockWasPickedUp)
    {
        for(auto &C : lastCoords) draw(C);
    }

    //draw it separately, overlapping anything that was their before...
    if(p_selectedBlock && (blockWasPickedUp || blockWasMoved))
    {
        for(int i = 0; i < 5; i++)
        {
            Coord C = p_selectedBlock->coords[i] + p_selectedBlock->defaultPos;
            if(isColliding) setCellColor(C, sf::Color(255,0,0,128), true);
            else setCellColor(C, sf::Color(0,255,0,128), true);

            //texture is of the block...
            int texId = p_selectedBlock->texIDs[i];
            int orientation = p_selectedBlock->orientations[i];
            int x_off = texId%4;
            int y_off = texId/4;
            bool flipped = p_selectedBlock->isFlipped;
            sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
            setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);
        }
    }

    if(!p_selectedBlock && idHover != INVALID_ID)
    {
        Coord P = m_blocks[idHover].defaultPos;
        for(auto &C : m_blocks[idHover].coords) draw(P+C);
    }
    if(hoverChanged && idLastHovered < 12) drawBlock(m_blocks[idLastHovered]);
    
    blockWasPlaced = false;
    blockWasMoved = false;
    blockWasPickedUp = false;

    window.clear(sf::Color::White);
    Grid::render(window);
    window.draw(text);
    //for(auto &T : idTexts) window.draw(T);
    window.display();
}

//so every time we call m_grid.setCell(C)
//that means that the cell at (C) is changed
//which means we need to update how it is to be rendered!

//and also when we move our ... we update the positions lastCoord...

//assume is valid COORD
void GameBoard::placeBlock(Pentamino &block)
{
    if(block.id == 6)
    {
        std::cout<<"FOO"<<std::endl;
    }
    Coord P = block.defaultPos;

    for(int i = 0; i < 5; i++)
    {
        Coord C = P + block.coords[i];
        m_data[mapID(C)] = (block.id + (i << 4));
    }

    blockWasPlaced = true;
}

//set as default all the old places...
void GameBoard::pickupBlock(Pentamino &block)
{
    Coord P = block.defaultPos;
    
    for(int i = 0; i < 5; ++i)
    {
        Coord C = P + block.coords[i];
        if(isInWinShape(C))
        {
            m_data[mapID(C)] = WINZONE_ID;
        }
        else
        {
            m_data[mapID(C)] = BACKGROUND_ID;
        }
    }

    blockWasPickedUp = true;
}

//or a shape... really
/*
void GameBoard::draw(Pentamino &block)
{
    Coord P = block.defaultPos;
    
    for(auto &C : block.coords)
    {
        draw(P+C);
    }
}*/

void GameBoard::draw(Coord C)
{
    if(!isValid(C)) return;

    unsigned int id = m_data[mapID(C)];
    bool inWinShape = isInWinShape(C);

    //colour variation at this location???
    srand((m_randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF));
    int variation = rand()%16;
    sf::Color color;

    if(inWinShape)
    {
        if(isAValidPlacement) color = sf::Color::White;
        else color = sf::Color(255,170,170);
    }
    else color = sf::Color::Black;
    color = variate(color, variation);
    setCellColor(C, color, false);

    if( (id&0x0F) > 12)
    {
        if(id == BACKGROUND_ID) setCellTexture(C, {256.f,128.f}, {128.f, 128.f}, rand()%4, rand()%2);
        else if(id == WINZONE_ID) setCellTexture(C, {384.f,0.f}, {128.f, 128.f}, rand()%4, rand()%2);
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
    setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);

    //color of the piece at the location
    HSL hsl;
    hsl.hue = m_blockHues[id];
    hsl.lightness = 0.4f;

    if(inWinShape)
    {
        hsl.saturation = 0.8f;
    }
    else hsl.saturation = 0.1f;

    if(won)
    {
        hsl.saturation = 0.9f;//*= 1.35f;
        hsl.lightness = 0.6f;//1.f;
    }

    if(id == idHover && p_selectedBlock == nullptr)
    {
        hsl.saturation += 0.1f;
        hsl.lightness += 0.1f;
    }

    color = fromHSL(hsl);
    color.a = 192;
    setCellColor(C, color, true);
}

void GameBoard::run()
{
    while(window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
}

void GameBoard::handleEvents()
{
    sf::Event event;
    window.waitEvent(event);
    {
    
    if(event.type == sf::Event::Closed) window.close();
    
    if(event.type == sf::Event::KeyPressed)
    {
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
    }
    if(event.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord = getCoordinate(mousePos);
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
            if(isValid(mouseCoord))
            {
                unsigned int id = m_data[mapID(mouseCoord)]&0x0F;
                hoverChanged = (id != idHover);
                idLastHovered = idHover;
                idHover = id; 
            }
            else idHover = INVALID_ID;
        }
    }
    if(event.type == sf::Event::MouseButtonPressed)
    {
        //get cell id and print out
        if(isValid(mouseCoord))
        {
            int id = m_data[mapID(mouseCoord)]&0x0F;
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

    }
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

//toggle a switch.. on the grid...
//toggle is if selected.
//save it!
void GameBoard::setWinShape(int lvl)
{
    //return;
    //load the level
    //srand(time(NULL));
    //unsigned int r = rand()%9;//10;//rand()%Level::m_preLoadedLevels.size();//8;//7;//rand()%8;
    //get a random level!
    Level L(lvl);

    int x_offset = (m_width - L.width)/2;
    //x_offset = 32 - x_offset;
    int y_offset = (m_height - L.height)/4 + 1;
    std::cout<<"x_ofsset is "<<x_offset<<std::endl;
    for(unsigned int i = 0; i < L.height; i++)
    {
        m_winzoneMap[i + y_offset] = L.data[i] << x_offset;
    }

    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(checkBit(m_winzoneMap[j],i))
            {
                //sf::Color color = blockColors[12];
                //m_grid.setCellColor({i,j},color);
                if(isValid({i,j}))
                    m_data[mapID({i,j})] = WINZONE_ID;
                m_winShapeCoords.push_back(Coord(i,j));
            }
        }

    return;
}
/*
void GameBoard::setWinShape()
{
    //int i, j;
    
    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(checkBit(m_winzoneMap[j],i))
            {
                sf::Color color = blockColors[12];
                setCellColor({i,j},color);
                m_data[mapID({i,j})].id = -1;
                winShapeCoords.push_back(Coord(i,j));
            }
        }
        
}*/

bool GameBoard::isInWinShape(Coord C)
{
    return checkBit(m_winzoneMap[C.j], C.i);
}

int GameBoard::floodFill(Coord C, bool *visited)
{
    if(!isValid(C)) return 0;
    if(visited[mapID(C)]) return 0;
    int id = m_data[mapID(C)];
    if(id != WINZONE_ID) return 0;
    visited[mapID(C)] = true;

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
    bool visited[m_width * m_height];
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