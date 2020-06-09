#ifndef GRID2D_SFML_HPP
#define GRID2D_SFML_HPP

//A square grid (not triangular, not hexagonal)

#include <SFML/Graphics.hpp>
#include <vector>

struct Coord
{
    Coord(int a = 0, int b = 0) : i(a), j(b) {}
    Coord operator + (const Coord &C) {return Coord(i + C.i, j + C.j);}
    bool operator != (const Coord &C) {return i != C.i || j != C.j;}
    int i, j;
};

typedef sf::Vector2f Vec2;

//assert about unsigned int ness later
//then can remove 2 of the conditions in isValid()
struct CoordMapper
{
    CoordMapper(int w=0, int h=0) : width(w), height(h) {}
    int width, height;
    unsigned int mapID(Coord C){return C.i + C.j * width;}
    bool isValid(Coord C){return (C.i < width && C.j < height && C.i >= 0 && C.j >= 0);}
    unsigned int sz(){return width * height;}
};

class Grid
{
public:
    Grid(int width = 10, int height = 10, float cellSize = 20.f);
    const int getWidth() const { return m_width; }
    const int getHeight() const { return m_height; }
    const sf::Vector2f getSize() const { return sf::Vector2f(m_width * m_cellSize, m_height * m_cellSize); }
    const float getCellSize() const {return m_cellSize;}
    Coord getCoordinate(const sf::Vector2f &pos);
    void render(sf::RenderTarget &target);
    void setTexMap(sf::Texture *tex) {m_texMap = tex;}
    
//protected:
//pull these 2 out ???
//or push the data in...
    inline unsigned int mapID(Coord C){return C.i + C.j * m_width;}
    inline bool isValid(Coord C){return (C.i < m_width && C.j < m_height && C.i >= 0 && C.j >= 0);}

    int m_width, m_height;

    void setCellColor(Coord C, sf::Color color, bool overlay = false);
    void rotateRight(Coord C);//or set rotation...
    void rotateLeft(Coord C);
    void setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz);
    void setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz, int orientation, bool isFlipped);
    void flip(Coord C);//y-axis
    void clear(sf::Color color);

private:
    std::vector<sf::Vertex> m_cellShapes;
    float m_cellSize;
    const sf::Texture *m_texMap;
    float m_texSz;
    uint32_t m_randSeed;
};

#endif //GRID2D_SFML_HPP