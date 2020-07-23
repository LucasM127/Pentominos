#include "Grid.hpp"
#include "Colors.hpp"
#include "LehmerRandom.hpp"

//this is a vertex mapper
Grid::Grid(unsigned int width, unsigned int height, float cellSize_x, float cellSize_y)
            : m_vbuffer(sf::Triangles, sf::VertexBuffer::Dynamic), m_cellSize(cellSize_x, cellSize_y), m_texMap(nullptr),
              m_texSz(128.f), m_mapper(width, height), m_x_offset(0.f), m_y_offset(0.f)
{
    srand(time(NULL));
    m_randSeed = rand();

    resize(width, height, cellSize_x, cellSize_y);
}

void Grid::resize(unsigned int width, unsigned int height, float cellSize_x, float cellSize_y)
{
    m_mapper.width = width;
    m_mapper.height = height;
    m_cellSize.x = cellSize_x;
    m_cellSize.y = cellSize_y;
    m_cellShapes.resize(m_mapper.sz() * 6);
    for(unsigned int j = 0; j < m_mapper.height; ++j)
    {
        for(unsigned int i = 0; i < m_mapper.width; ++i)
        {
            uint cellID = m_mapper.mapID({i,j});
            m_cellShapes[6 * cellID + 0].position = {i * m_cellSize.x, j * m_cellSize.y};
            m_cellShapes[6 * cellID + 1].position = {i * m_cellSize.x, (j+1) * m_cellSize.y};
            m_cellShapes[6 * cellID + 2].position = {(i+1) * m_cellSize.x, j * m_cellSize.y};
            
            m_cellShapes[6 * cellID + 3].position = {(i+1) * m_cellSize.x, j * m_cellSize.y};
            m_cellShapes[6 * cellID + 4].position = {i * m_cellSize.x, (j+1) * m_cellSize.y};
            m_cellShapes[6 * cellID + 5].position = {(i+1) * m_cellSize.x, (j+1) * m_cellSize.y};
        }
    }

    m_vbuffer.create(m_cellShapes.size());
    amModified = true;
}

void Grid::clear(sf::Color color)
{
    for(unsigned int j = 0; j < m_mapper.height; ++j)
    {
        for(unsigned int i = 0; i < m_mapper.width; ++i)
        {
            setCellColor({i,j},color);
//hmm???
            setCellTexture({i,j}, {256.f,128.f}, {128.f, 128.f});
        }
    }
    amModified = true;
}

//set the variation here ??? again
//and the texture rotation too ???
void Grid::setCellColor(Coord C, sf::Color color, bool overlaps)
{
    //if(!m_mapper.isValid(C))
    if(!m_mapper.isValid(C)) return;
    unsigned int id = m_mapper.mapID(C);
    id *= 6;
    
    if(overlaps) color = COLOR::alphaBlend(color, m_cellShapes[id].color);
    else //do the random thing here!
    {
        uint32_t seed = (m_randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF);
        lsrand(seed);
        int variation = lrand()%16;
        color = COLOR::variate(color, variation);
    }
    for(int i = 0; i<6; i++) m_cellShapes[id+i].color = color;

    amModified = true;
}

void Grid::flip(Coord C)
{
    if(!m_mapper.isValid(C)) return;
    unsigned int cellID = m_mapper.mapID(C);
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
    if(!m_mapper.isValid(C)) return;
    unsigned int cellID = m_mapper.mapID(C);
    sf::Vector2f temp = m_cellShapes[6 * cellID + 0].texCoords;
    m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 2].texCoords;
    m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 3].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
    m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 1].texCoords;
    m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 4].texCoords = temp;
}

void Grid::rotateLeft(Coord C)
{
    if(!m_mapper.isValid(C)) return;
    unsigned int cellID = m_mapper.mapID(C);
    sf::Vector2f temp = m_cellShapes[6 * cellID + 0].texCoords;
    m_cellShapes[6 * cellID + 0].texCoords = m_cellShapes[6 * cellID + 1].texCoords;
    m_cellShapes[6 * cellID + 1].texCoords = m_cellShapes[6 * cellID + 4].texCoords = m_cellShapes[6 * cellID + 5].texCoords;
    m_cellShapes[6 * cellID + 5].texCoords = m_cellShapes[6 * cellID + 2].texCoords;
    m_cellShapes[6 * cellID + 2].texCoords = m_cellShapes[6 * cellID + 3].texCoords = temp;
}
//random function here...
void Grid::setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz, int orientation, bool isFlipped)
{
    if(!m_mapper.isValid(C)) return;
    unsigned int cellID = m_mapper.mapID(C);

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

    amModified = true;
}

void Grid::setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz)
{
    //rand functin
    uint32_t seed = (m_randSeed & 0xFF) << 24 |(C.i & 0xFFF) << 12 | (C.j & 0xFFF);
    lsrand(seed);
    int o = lrand()%8;
    setCellTexture(C, uv, sz, (o & 0b11), (o & 0b100));
}

//does it even make a difference ?
void Grid::render(sf::RenderTarget &target)
{
    sf::RenderStates states;
    states.texture = m_texMap;
    sf::Transform T;
    T.translate(m_x_offset,m_y_offset);
    states.transform = T;
//just tinyest bit faster !
#ifndef VBUFFER
    if(amModified)
    {
        m_vbuffer.update(&m_cellShapes[0]);
        amModified = false;
    }
    target.draw(m_vbuffer, states);
#else
    target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles, states);
#endif
    
    //optionally with texMap...
    
    //target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles, states);
    //else target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles);
}
//if get coord from here... only, is invalid if get anywhere else!
Coord Grid::getCoordinate(const sf::Vector2f &pos)
{
    return Coord(   (pos.x - m_x_offset) / m_cellSize.x,
                    (pos.y - m_y_offset) / m_cellSize.y);
}