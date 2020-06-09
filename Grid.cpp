#include "Grid.hpp"
#include "Colors.hpp"
#include "LehmerRandom.hpp"

//this is a vertex mapper
Grid::Grid(int width, int height, float cellSize)
            : m_width(width), m_height(height), m_cellSize(cellSize), m_texMap(nullptr)
{
    srand(time(NULL));
    m_randSeed = rand();//hacky

    m_texSz = 128.f;
    
    m_cellShapes.resize(m_width * m_height * 6);
    for(int j = 0; j < m_height; ++j)
    {
        for(int i = 0; i < m_width; ++i)
        {
            uint cellID = mapID({i,j});
            m_cellShapes[6 * cellID + 0].position = {i * m_cellSize, j * m_cellSize};
            m_cellShapes[6 * cellID + 1].position = {i * m_cellSize, (j+1) * m_cellSize};
            m_cellShapes[6 * cellID + 2].position = {(i+1) * m_cellSize, j * m_cellSize};
            
            m_cellShapes[6 * cellID + 3].position = {(i+1) * m_cellSize, j * m_cellSize};
            m_cellShapes[6 * cellID + 4].position = {i * m_cellSize, (j+1) * m_cellSize};
            m_cellShapes[6 * cellID + 5].position = {(i+1) * m_cellSize, (j+1) * m_cellSize};
        }
    }
}

void Grid::clear(sf::Color color)
{
    for(int j = 0; j < m_height; ++j)
    {
        for(int i = 0; i < m_width; ++i)
        {
            setCellColor({i,j},color);
//hmm???
            setCellTexture({i,j}, {256.f,128.f}, {128.f, 128.f});
        }
    }
}

    /***
    {//Calculating and seeding spatial random function
    //(x & 0xFFFF) << 16 | (y & 0xFFFF); //copypasta OLC
    //8bits + 12bits + 12bits = 32bit uint_32
    srand((randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF));
    }

    if(v) color = variate(color, rand()%16);
    color = alphaBlend(color, m_cellShapes[id].color);
***/
//set the variation here ??? again
//and the texture rotation too ???
void Grid::setCellColor(Coord C, sf::Color color, bool overlaps)
{
    if(!isValid(C)) return;
    unsigned int id = mapID(C);
    id *= 6;
    
    if(overlaps) color = alphaBlend(color, m_cellShapes[id].color);
    else //do the random thing here!
    {
        uint32_t seed = (m_randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF);
        lsrand(seed);
        int variation = lrand()%16;
        color = variate(color, variation);
    }
    for(int i = 0; i<6; i++) m_cellShapes[id+i].color = color;
}

void Grid::flip(Coord C)
{
    if(!isValid(C)) return;
    unsigned int cellID = mapID(C);
    m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 0].texCoords;
    m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 4].texCoords;
    m_cellShapes[6 * cellID + 4].texCoords = m_cellShapes[6 * cellID + 1].texCoords;

    m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
    m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 3].texCoords;
    m_cellShapes[6 * cellID + 3].texCoords = m_cellShapes[6 * cellID + 2].texCoords;
}

//hmmmmmmmmm
void Grid::rotateRight(Coord C)
{
    if(!isValid(C)) return;
    unsigned int cellID = mapID(C);
    sf::Vector2f temp = m_cellShapes[6 * cellID + 0].texCoords;
    m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 2].texCoords;
    m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 3].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
    m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 1].texCoords;
    m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 4].texCoords = temp;
}

void Grid::rotateLeft(Coord C)
{
    if(!isValid(C)) return;
    unsigned int cellID = mapID(C);
    sf::Vector2f temp = m_cellShapes[6 * cellID + 0].texCoords;
    m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 1].texCoords;
    m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 4].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
    m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 2].texCoords;
    m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 3].texCoords = temp;
}
//random function here...
void Grid::setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz, int orientation, bool isFlipped)
{
    if(!isValid(C)) return;
    unsigned int cellID = mapID(C);

    //0 - 4 and if is flipped ?
    switch (orientation)
    {
    case 0://default
        m_cellShapes[6 * cellID + 0].texCoords = {uv.x,uv.y};//A
        m_cellShapes[6 * cellID + 1].texCoords = {uv.x,uv.y + sz.y};//C
        m_cellShapes[6 * cellID + 2].texCoords = {uv.x + sz.x,uv.y};//B
        
        m_cellShapes[6 * cellID + 3].texCoords = {uv.x + sz.x,uv.y};//B
        m_cellShapes[6 * cellID + 4].texCoords = {uv.x,uv.y + sz.y};//C
        m_cellShapes[6 * cellID + 5].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        break;
    case 1://90 ccw
        m_cellShapes[6 * cellID + 0].texCoords = {uv.x + sz.x,uv.y};//B
        m_cellShapes[6 * cellID + 1].texCoords = {uv.x,uv.y};//A
        m_cellShapes[6 * cellID + 2].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        
        m_cellShapes[6 * cellID + 3].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        m_cellShapes[6 * cellID + 4].texCoords = {uv.x,uv.y};//A
        m_cellShapes[6 * cellID + 5].texCoords = {uv.x,uv.y + sz.y};//C
        break;
    case 2://180
        m_cellShapes[6 * cellID + 0].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        m_cellShapes[6 * cellID + 1].texCoords = {uv.x + sz.x,uv.y};//B
        m_cellShapes[6 * cellID + 2].texCoords = {uv.x,uv.y + sz.y};//C
        
        m_cellShapes[6 * cellID + 3].texCoords = {uv.x,uv.y + sz.y};//C
        m_cellShapes[6 * cellID + 4].texCoords = {uv.x + sz.x,uv.y};//B
        m_cellShapes[6 * cellID + 5].texCoords = {uv.x,uv.y};//A
        break;
    case 3://90 cw
        m_cellShapes[6 * cellID + 0].texCoords = {uv.x,uv.y + sz.y};//C
        m_cellShapes[6 * cellID + 1].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        m_cellShapes[6 * cellID + 2].texCoords = {uv.x,uv.y};//A
        
        m_cellShapes[6 * cellID + 3].texCoords = {uv.x,uv.y};//A
        m_cellShapes[6 * cellID + 4].texCoords = {uv.x + sz.x, uv.y + sz.y};//D
        m_cellShapes[6 * cellID + 5].texCoords = {uv.x + sz.x,uv.y};//B
        break;
    
    default:
        break;
    }

    if(isFlipped)
    {
        m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 0].texCoords;
        m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 3].texCoords;
        m_cellShapes[6 * cellID + 3].texCoords = m_cellShapes[6 * cellID + 2].texCoords;

        m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
        m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 4].texCoords;
        m_cellShapes[6 * cellID + 4].texCoords = m_cellShapes[6 * cellID + 1].texCoords;
    }
}

void Grid::setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz)
{
    //rand functin
    uint32_t seed = (m_randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF);
    lsrand(seed);
    int o = lrand()%8;
    setCellTexture(C, uv, sz, (o & 0b11), (o & 0b100));
}

void Grid::render(sf::RenderTarget &target)
{
    sf::RenderStates states;
    states.texture = m_texMap;
    //optionally with texMap...
    target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles, states);
    //else target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles);
}
//if get coord from here... only, is invalid if get anywhere else!
Coord Grid::getCoordinate(const sf::Vector2f &pos)
{
    return Coord(   pos.x / m_cellSize,
                    pos.y / m_cellSize);
}