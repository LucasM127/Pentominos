#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <SFML/Graphics.hpp>
#include <functional>
#include "Grid.hpp"
#include "Colors.hpp"

namespace PGUI
{

//not really a gui button but convenience functions
struct Button
{
    unsigned int min_x, max_x, min_y, max_y;
    sf::Color color;
    void set(unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy);
    void setColor(Grid &grid);
    void brighten(Grid &grid);
    bool isHovered(Coord C);
};

class TextBox
{
public:
    TextBox(sf::Window &_parent, const std::wstring & title, sf::Font &font, std::function<void()> fn);
    std::string getString(){return m_string;}
    ~TextBox(){}
private:
    sf::Window &parent;
    sf::RenderWindow m_window;

    std::function<void()> m_function;

    Grid m_grid;

    std::string m_string;
    sf::Text m_text;
    sf::Text m_cursor;
    sf::RectangleShape m_textBgShape;

    //buttons
    Button OK, Cancel;
    sf::Text m_OKtext;
    sf::Text m_Canceltext;

    void run();
    void parentMSGLoop();
};


}//namespace PGUI

#endif //TEXTBOX_HPP