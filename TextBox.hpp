#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

//a really bad gui for a singular purpose to fit the look of the game
//developed enough to just work for what i need

#include <SFML/Graphics.hpp>
#include <functional>
#include "Grid.hpp"
#include "Colors.hpp"
#include "GameState.hpp"
//#include "Level.hpp"

namespace PGUI
{

void setCellSize(float sz);
//dialog input basic shit
enum class MSG : unsigned int
{
    CLOSED = 0,
    YES,//confirmation
    NO,//confirmation
    TEXT_CHANGED,//text box
    DELETE,//delete an entry
    MOVE,//move level to another folder
    LIST_ITEM_CHOSE//list box entry chosen
};

const std::string MSGStrings[7] = 
{
    "Closed",
    "Yes",
    "No",
    "Text Changed",
    "Delete",
    "Move",
    "List Item Chose"
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

//try and make it not a window???
//is that possible?
//just a grid...
//use the gamestate render then own render pos...
class DialogBox
{
public:
    DialogBox(uint width, uint height, GameState &_parent, sf::RenderWindow &_window, sf::Font &_font, const std::wstring & title);//just draw a 3x3 box right now
    virtual ~DialogBox(){}
    GameState &state;
    sf::RenderWindow &window;
    sf::Font &font;
    Grid m_grid;
    sf::Text m_titleText;
    sf::Transform m_transform;//for drawing all child objects. just linear scaling
    Button TitleBar;
    Button Close;

    MSG run();
    virtual void render(){}
    virtual void handleEvent(sf::Event &event){}
    virtual void cleanUp(){}
    virtual void update(){}
    MSG msg;
    bool isRunning;

    ViewRect m_viewRect;
};

class TextBoxDB : public DialogBox //public Dialog box...
{
public:
    TextBoxDB(GameState &_state, sf::RenderWindow &_parent, const std::wstring & title, const std::string &string, sf::Font &font);
    void render() override;
    void handleEvent(sf::Event &event) override;
    void cleanUp() override;
    void update() override;
    std::string getString();
    ~TextBoxDB(){}
private:
    bool cursorIsShowing;
    std::string m_string, m_string2;
    sf::Text m_text, m_text2;
    sf::Text m_cursor;
    sf::RectangleShape m_textBgShape;

    Coord mouseCoord;
    Button *hoveredButton = nullptr;
    Button *lastHovered = nullptr;
    float ticker;
    sf::Clock clock;

    //buttons
    Button OK, Cancel, Delete, Move;
    sf::Text m_OKtext;
    sf::Text m_Canceltext;
    sf::Text m_Deletetext;
    sf::Text m_Movetext;

    sf::Text m_instructionText;
};

class ListBoxDB : public DialogBox
{
public:
    ListBoxDB(GameState &_state, sf::RenderWindow &_parent, const std::wstring &title, sf::Font &font, std::vector<std::string> &names);
    std::vector<sf::Text> m_texts;
    unsigned int getChosenId(){return activeId;}
    void render() override;
    void handleEvent(sf::Event &event) override;
private:
    std::vector<std::string> &names;
    Coord activeCoord;
    Coord lastCoord;
    unsigned int numEntriesShown;
    unsigned int offset;
    unsigned int activeId;
    void scroll(int amt);
    void updateId(unsigned int id);
};

/*****

class TextBox//public Dialog box...
{
public:
    TextBox(sf::Window &_parent, const std::wstring & title, const std::string &string, sf::Font &font);//, std::function<void()> fn);
    MSG run();
    std::string getString();
    ~TextBox(){}
private:
    sf::Window &parent;
    sf::RenderWindow m_window;

//    std::function<void()> m_function;

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
    ListBox(sf::Window &_parent, const std::string &title, sf::Font &font, std::vector<std::string> &names);//, const std::string &title);//some function???
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
    std::vector<std::string> &names;
    Coord activeCoord;
    Coord lastCoord;
    unsigned int numEntriesShown;
    unsigned int offset;
    unsigned int activeId;
};
*/

}//namespace PGUI

#endif //TEXTBOX_HPP