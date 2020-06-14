#include "EditBoard.hpp"
#include "Pentaminos.hpp"//use logic to determine which texture to use depending on boundaries.
//free function put here for now
#include <fstream>
//some way to save ORIENTATION AND TEXID INFO with the coords....
void setGridCoordTexBasedOnDataAtCoord(Grid &grid, std::vector<uint32_t> &data, Coord C)
{
    //assumes C is a valid coordinate.
    enum DIR {UP, LEFT, DOWN, RIGHT};

    bool neigbour[4] = {false, false, false, false};

    //fill neighbours
    Coord up(C.i, C.j-1);
    Coord down(C.i, C.j+1);
    Coord left(C.i-1, C.j);
    Coord right(C.i+1, C.j);

    auto checkCoord = [&](Coord x)->bool
    {
        if(!grid.isValid(x))
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

    if(!checkCoord(C))
    {
        texId = BACKGROUND_TEXTURE;
        orientation = rots[0];//rand()%4];
    }//assumed??? lol

    //set the grid texture accordingly...
    int x_off = texId%4;
    int y_off = texId/4;
    sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
    grid.setCellTexture(C, uvpos, {128.f, 128.f}, orientation, false);
}

EditBoard::EditBoard(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    grid.setTexMap(context.texture);
    int m_height = grid.getHeight();
    int m_width = grid.getWidth();
    m_data.resize(m_height, 0);
    window.setTitle("Pentaminos");
//    window.create(sf::VideoMode(1320, 840), "Editor", sf::Style::Close);

    std::ifstream file("foo", std::ios::out | std::ios::binary);

    for(auto &i : m_data)
    {
        if(file) file.read((char*)&i, sizeof(uint32_t));//file >> i;
        else break;
    }
    file.close();

    for(int i =0; i< m_width; i++)
        for(int j= 0; j<m_height; j++)
            draw(Coord(i,j));
    
    for(int i =0; i< m_width; i++)
        for(int j= 0; j<m_height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j));
}
/*
void EditBoard::run()
{
    while(window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
}
*/
void EditBoard::update()
{
    int m_height = grid.getHeight();
    int m_width = grid.getWidth();
//    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for(int i =0; i< m_width; i++)
        for(int j= 0; j<m_height; j++)
            setGridCoordTexBasedOnDataAtCoord(grid, m_data, Coord(i,j));
}
/*
void EditBoard::render()
{
    window.clear(sf::Color::White);
    Grid::render(window);
    window.display();
}*/

void EditBoard::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
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
//flood fill each letter a random color...
void EditBoard::draw(Coord C)
{
    if(checkBit(m_data[C.j], C.i))
                grid.setCellColor(C, sf::Color(255 * C.i / 22,255 * C.j / 13,128));//sf::Color::White);
    else grid.setCellColor(C, sf::Color::Black);
}

void EditBoard::save()
{
    {
    std::ofstream file("foo", std::ios::out | std::ios::binary);
    for(auto &i : m_data) file.write((char*)&i, sizeof(uint32_t));
    file.close();
    }/*
    {
    std::ofstream file("MainPage", std::ios::out | std::ios::binary);
    file.write((char*) grid.getWidth(), sizeof(int));//utility class
    file.write((char*) grid.getHeight(), sizeof(int));//make this write load a LOT easier...
    for(int i =0; i< grid.getWidth(); i++)
        for(int j= 0; j<grid.getHeight(); j++)
        {
            uint16_t data = 0;
//            data = //orientation, and the texture at each coordinate... save it???
//            file.write((char*) checkBit(m_data[j], i), 1);
        }
    }*/
}

//copypasta from GameBoard...
/*
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
}*/