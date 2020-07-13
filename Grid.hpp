#ifndef GRID2D_SFML_HPP
#define GRID2D_SFML_HPP

//A square grid (not triangular, not hexagonal)
//adding a vertexBuffer????

#include <SFML/Graphics.hpp>
#include <vector>

extern bool renderTestBool;

struct Coord
{
    Coord(unsigned int a = 0, unsigned int b = 0) : i(a), j(b) {}
    Coord operator + (const Coord &C) {return Coord(i + C.i, j + C.j);}
    bool operator != (const Coord &C) {return i != C.i || j != C.j;}
    unsigned int i, j;
};//negative coordinates?  need an offset in the coord mapper

typedef sf::Vector2f Vec2;

//map a coord to a vector
struct CoordMapper
{
    CoordMapper(unsigned int w=0, unsigned int h=0) : width(w), height(h) {}
    unsigned int width, height;
    inline unsigned int mapID(Coord C) const {return C.i + C.j * width;}
    inline bool isValid(Coord C) const {return (C.i < width && C.j < height);}
    inline unsigned int sz() const {return width * height;}
};

class Grid
{
public:
    Grid(unsigned int width = 10, unsigned int height = 10, float cellSize_x = 20.f, float cellSize_y = 20.f);
    void create(unsigned int width, unsigned int height, float cellSize_x, float cellSize_y);
    void setOffset(float x, float y){m_x_offset = x; m_y_offset = y;}
    const unsigned int getWidth() const { return m_mapper.width; }
    const unsigned int getHeight() const { return m_mapper.height; }
    const CoordMapper &getMapper() const { return m_mapper; }
    const sf::Vector2f getSize() const { return sf::Vector2f(m_mapper.width * m_cellSize.x, m_mapper.height * m_cellSize.y); }
    const sf::Vector2f getOffset() const { return sf::Vector2f(m_x_offset, m_y_offset); }
    const sf::Vector2f getCellSize() const {return m_cellSize;}
    Coord getCoordinate(const sf::Vector2f &pos);
    void render(sf::RenderTarget &target);
    void setTexMap(sf::Texture *tex) {m_texMap = tex;}
    
//the mapping function...
//inherit the grid,
//own a mapper -> that we can pass?
    //inline unsigned int mapID(Coord C){return C.i + C.j * m_width;}
    //inline bool isValid(Coord C){return (C.i < m_width && C.j < m_height && C.i >= 0 && C.j >= 0);}

    //int m_width, m_height;

    void setCellColor(Coord C, sf::Color color, bool overlay = false);
    void rotateRight(Coord C);//or set rotation...
    void rotateLeft(Coord C);
    void setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz);
    void setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz, int orientation, bool isFlipped);
    void flip(Coord C);//y-axis
    void clear(sf::Color color);

private:
    sf::VertexBuffer m_vbuffer;
    bool amModified;
    std::vector<sf::Vertex> m_cellShapes;
    sf::Vector2f m_cellSize;
    const sf::Texture *m_texMap;
    float m_texSz;
    uint32_t m_randSeed;
    CoordMapper m_mapper;

    float m_x_offset, m_y_offset;
};

#endif //GRID2D_SFML_HPP