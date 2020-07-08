#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

//a really bad gui for a singular purpose to fit the look of the game
//developed enough to just work for what i need

#include <SFML/Graphics.hpp>
#include <functional>
#include "Grid.hpp"
#include "Colors.hpp"
#include "Level.hpp"

namespace PGUI
{
//dialog input basic shit
enum class MSG
{
    CLOSED,
    YES,//confirmation
    NO,//confirmation
    TEXT_CHANGED,//text box
    DELETE,//delete an entry
    MOVE,//move level to another folder
    LIST_ITEM_CHOSE//list box entry chosen
};

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

class TextBox //public Dialog box...
{
public:
    TextBox(sf::Window &_parent, const std::wstring & title, const std::string &string, sf::Font &font);//, std::function<void()> fn);
    MSG run();
    std::string getString();
    ~TextBox(){}
private:
    sf::Window &parent;
    sf::RenderWindow m_window;

    std::function<void()> m_function;

    Grid m_grid;

    std::string m_string;
    sf::Text m_text, m_text2;
    sf::Text m_cursor;
    sf::RectangleShape m_textBgShape;

    //buttons
    Button OK, Cancel, Delete, Move;
    sf::Text m_OKtext;
    sf::Text m_Canceltext;
    sf::Text m_Deletetext;
    sf::Text m_Movetext;

    sf::Text m_instructionText;

    void parentMSGLoop();
};

class ListBox //public Dialog...
{
public:
    ListBox(sf::Window &_parent, const std::string &title, sf::Font &font, std::vector<Level> &levels, const std::string &fileName);//some function???
    std::vector<sf::Text> m_texts;
//    std::vector<sf::RectangleShape> m_shapes;
    unsigned int getChosenId(){return activeId;}
    MSG run();
private:
//INHERIT THIS FROM BASE DIALOG CONTROL???
//Dialog in a dialog,,, change the parentMSGLoop -> default loop... if(parent) -> parent->basicMessage()
//void basicMSGLoop();
    sf::Window &parent;
    sf::RenderWindow m_window;
    Grid m_grid;//....
    void parentMSGLoop();
    std::vector<Level> &levels;
    Coord activeCoord;
    Coord lastCoord;
    unsigned int numEntriesShown;
    unsigned int offset;
    unsigned int activeId;
};


}//namespace PGUI

#endif //TEXTBOX_HPP