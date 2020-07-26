#include "WelcomeState.hpp"
#include "Pentaminos.hpp"

//use logic to determine which texture to use depending on boundaries.
//free function put here for now
#include <fstream>
//some way to save ORIENTATION AND TEXID INFO with the coords....
void setGridCoordTexBasedOnDataAtCoord(Grid &grid, std::vector<uint32_t> &data, Coord C, ViewRect &viewRect)
{
    //assumes C is a valid coordinate.
    enum DIR {UP, LEFT, DOWN, RIGHT};

    bool neigbour[4] = {false, false, false, false};

    //fill neighbours
    Coord up(C.i, C.j-1);
    Coord down(C.i, C.j+1);
    Coord left(C.i-1, C.j);
    Coord right(C.i+1, C.j);

    CoordMapper CM;
    CM.width = viewRect.width;
    CM.height = viewRect.height;

    auto checkCoord = [&](Coord x)->bool
    {//not the grid though!!!!
        //if(!grid.getMapper().isValid(x))
        //x.i -= viewRect.P.i;
        //x.j -= viewRect.P.j;
        if(!CM.isValid(x))
            return false;
        return checkBit(data[x.j], x.i);
    };

    neigbour[ UP ]  = checkCoord(up);
    neigbour[DOWN]  = checkCoord(down);
    neigbour[LEFT]  = checkCoord(left);
    neigbour[RIGHT] = checkCoord(right);

    TexAtlasID texId;
    Orientation orientation;

    int numNeighbours = 0;
    if(neigbour[UP]) numNeighbours++;
    if(neigbour[DOWN]) numNeighbours++;
    if(neigbour[LEFT]) numNeighbours++;
    if(neigbour[RIGHT]) numNeighbours++;

    if(numNeighbours == 4)
    {
        texId = INSIDE_PIECE;
        orientation = rots[rand()%4];//any orientation
    }
    else if(numNeighbours == 0)
    {//no neighbours
        texId = WINZONE_TEXTURE;
        orientation = rots[rand()%4];//any orientation
    }
    else if(numNeighbours == 1)
    {
        texId = END_WALLS;// up default
        //which neighbour?
        if(neigbour[0]) orientation = rots[0];
        if(neigbour[1]) orientation = rots[1];
        if(neigbour[2]) orientation = rots[2];
        if(neigbour[3]) orientation = rots[3];
    }
    else if(numNeighbours == 3)
    {
        texId = SINGLE_WALL;// to the left default (1) -> 0 , 2 -> 1 etc...
        //which neighbour?
        if(!neigbour[0]) orientation = rots[3];
        if(!neigbour[1]) orientation = rots[0];
        if(!neigbour[2]) orientation = rots[1];
        if(!neigbour[3]) orientation = rots[2];
    }
    else//numNeighbours == 2
    {
        //either channelWall or angle wall
        if(neigbour[UP] && neigbour[DOWN])
        {
            texId = CHANNEL_WALLS;
            orientation = DEFAULT;//rots[0] or rots[2]
        }
        if(neigbour[LEFT] && neigbour[RIGHT])
        {
            texId = CHANNEL_WALLS;
            orientation = rots[1];//or 3
        }
        if(neigbour[UP] && neigbour[RIGHT])
        {
            texId = ANGLE_WALLS;
            orientation = rots[0];
        }
        if(neigbour[UP] && neigbour[LEFT])
        {
            texId = ANGLE_WALLS;
            orientation = rots[1];
        }
        if(neigbour[DOWN] && neigbour[LEFT])
        {
            texId = ANGLE_WALLS;
            orientation = rots[2];
        }
        if(neigbour[DOWN] && neigbour[RIGHT])
        {
            texId = ANGLE_WALLS;
            orientation = rots[3];
        }
    }

    bool isBG = false;
    if(!checkCoord(C))
    {
        return;
        isBG = true;
        texId = BACKGROUND_TEXTURE;
    }//assumed??? lol

    //set the grid texture accordingly...
    int x_off = texId%4;
    int y_off = texId/4;
    sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
    Coord C_grid = viewRect.transform(C);
    if(isBG) grid.setCellTexture(C_grid, uvpos, {128.f,128.f});
    else
        grid.setCellTexture(C_grid, uvpos, {128.f, 128.f}, orientation, false);
}

//set the display Rect at constructor stage too!!!
WelcomeState::WelcomeState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    const Level &level = Level::intro;
    grid.setTexMap(context.texture);//uhuh
    uint height = m_viewRect.height;//grid.getHeight();
    uint width = m_viewRect.width;// grid.getWidth();

    uint x_offset = (width - level.width)/2;
    uint y_offset = (height - level.height)/2;

    m_data.resize(height, 0);
    for(unsigned int i = 0; i < level.height; i++)
    {
        uint32_t data = reverseBits(level.data[i], level.width);
        m_data[i + y_offset] = data << x_offset;
    }
    window.setTitle(level.name);

    paint();
    
}//think it...

void WelcomeState::paint()
{//passing it local coordinates...
    for(uint i =0; i< m_viewRect.width; i++)
        for(uint j= 0; j<m_viewRect.height; j++)
            draw( Coord(i,j) );
    
    for(uint i =0; i< m_viewRect.width; i++)
        for(uint j= 0; j<m_viewRect.height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j), m_viewRect);
    /*
    for(uint i =m_viewRect.P.i; i< m_viewRect.P.i + m_viewRect.width; i++)
        for(uint j= m_viewRect.P.j; j<m_viewRect.P.j + m_viewRect.height; j++)
            draw( Coord(i,j) );
    
    for(uint i =m_viewRect.P.i; i< m_viewRect.P.i + m_viewRect.width; i++)
        for(uint j= m_viewRect.P.j; j<m_viewRect.P.j + m_viewRect.height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j), m_viewRect);
            */
    /*
    for(uint i =0; i< width; i++)
        for(uint j= 0; j<height; j++)
            draw( Coord(i,j) );
    
    for(uint i =0; i< width; i++)
        for(uint j= 0; j<height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j));
            */
}

/*
void EditBoard::update()
{
    int height = grid.getHeight();
    int width = grid.getWidth();
//    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for(int i =0; i< width; i++)
        for(int j= 0; j<height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j));
}*/

void WelcomeState::handleEvent(const sf::Event &event)
{
    if(/*event.type == sf::Event::KeyPressed || */event.type == sf::Event::MouseButtonPressed)
        requestStateChange(MENU);
//    sf::Event event;
//    window.waitEvent(event);
/*
    
    if(event.type == sf::Event::Closed)
    {
        save();
        window.close();
    }
    
    if(event.type == sf::Event::MouseMoved)
    {
        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord = grid.getCoordinate(mousePos);
    }
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(grid.isValid(mouseCoord))
        {
            toggleBit(m_data[mouseCoord.j], mouseCoord.i);
            draw(mouseCoord);
        }
    }*/
}

//colour
void WelcomeState::draw(Coord C)
{
    Coord C_grid = m_viewRect.transform(C);
    if(checkBit(m_data[C.j], C.i))
//                grid.setCellColor(C, sf::Color(64,64,64,160));
                grid.setCellColor(C_grid, sf::Color(255 * C.i / 22,255 * C.j / 13,128));//sf::Color::White);
    else grid.setCellColor(C_grid, sf::Color::Black);
}