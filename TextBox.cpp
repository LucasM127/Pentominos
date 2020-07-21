#include "TextBox.hpp"

#include <cmath> //fmodf

namespace PGUI
{

//GLobals
float cellSz = 32.f;
unsigned int textSz = cellSz * 0.75f;//24

void setCellSize(float sz)
{
    cellSz = sz;
    textSz = 0.75f * cellSz;
}

void Button::set(unsigned int minx, unsigned int maxx, unsigned int miny, unsigned int maxy)
{
    min_x = minx;
    max_x = maxx;
    min_y = miny;
    max_y = maxy;
}

void Button::setColor(Grid &grid)
{
    for(unsigned int x = min_x; x <= max_x; ++x)
        for(unsigned int y = min_y; y <= max_y; ++y)
            grid.setCellColor({x,y}, color);
}

void Button::brighten(Grid &grid)
{
    sf::Color c = COLOR::brighten(color, 2.0f);
    for(unsigned int x = min_x; x <= max_x; ++x)
        for(unsigned int y = min_y; y <= max_y; ++y)
            grid.setCellColor({x,y}, c);
}

bool Button::isHovered(Coord C)
{
    return (C.i >= min_x && C.i <= max_x && C.j >= min_y && C.j <= min_y);
}

//TEXTBOX

TextBox::TextBox(sf::Window &_parent, const std::wstring & title, const std::string & string, sf::Font &font)//, std::function<void()> fn)
    : parent(_parent), m_grid(12,3,cellSz,cellSz), m_string(string)//m_function(fn), m_grid(12,3,40.f)
{
    for(auto &c : m_string) if(c == '\n') c = '^';
    std::string m_string2;

    m_instructionText.setFont(font);
    m_instructionText.setCharacterSize(textSz);
    m_instructionText.setPosition(0.f, 0.f);
    m_instructionText.setString("Type '^' for a newline character");
    m_instructionText.setFillColor(sf::Color(128,128,128,128));

    m_text.setFont(font);
    m_text.setCharacterSize(textSz);
    m_text.setPosition(0.f, cellSz);
    m_text.setString(m_string);
    m_text.setFillColor(sf::Color::Black);

    m_cursor.setFont(font);
    m_cursor.setString("|");
    m_cursor.setCharacterSize((int)cellSz);
    m_cursor.setFillColor(sf::Color(255,255,255,128));
    m_cursor.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width, 
                         m_text.getPosition().y - 5.f);
    
    m_text2.setFont(font);
    m_text2.setCharacterSize(textSz);
    m_text2.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width + m_cursor.getGlobalBounds().width, 
                         m_text.getPosition().y);
    //m_text2.setString(m_string2);
    m_text2.setFillColor(sf::Color::Black);

    m_grid.clear(sf::Color::White);
    for(unsigned int x = 0; x < m_grid.getWidth(); ++x)
        m_grid.setCellColor({x,1},sf::Color(192,192,192));
    OK.set(6,8,2,2);
    OK.color = COLOR::brighten(sf::Color::Green, 0.5f);
    OK.setColor(m_grid);
    Cancel.set(9,11,2,2);
    Cancel.color = COLOR::brighten(sf::Color::Yellow, 0.5f);
    Cancel.setColor(m_grid);
    Delete.set(0,2,2,2);
    Delete.color = COLOR::brighten(sf::Color::Red, 0.5f);
    Delete.setColor(m_grid);
    Move.set(3,5,2,2);
    Move.color = COLOR::brighten(sf::Color::Cyan, 0.5f);
    Move.setColor(m_grid);

    m_OKtext.setFont(font);
    m_OKtext.setCharacterSize(textSz);
    m_OKtext.setPosition(OK.min_x * cellSz + 0.f, OK.min_y * cellSz + 0.f);
    m_OKtext.setFillColor(sf::Color(0,0,0,128));
    m_OKtext.setString("ENTER");

    m_Canceltext.setFont(font);
    m_Canceltext.setCharacterSize(textSz);
    m_Canceltext.setPosition(Cancel.min_x * cellSz + 0.f, Cancel.min_y * cellSz + 0.f);
    m_Canceltext.setFillColor(sf::Color(0,0,0,128));
    m_Canceltext.setString("CANCEL");

    m_Deletetext.setFont(font);
    m_Deletetext.setCharacterSize(textSz);
    m_Deletetext.setPosition(Delete.min_x * cellSz + 0.f, Delete.min_y * cellSz + 0.f);
    m_Deletetext.setFillColor(sf::Color(0,0,0,128));
    m_Deletetext.setString("DELETE");

    m_Movetext.setFont(font);
    m_Movetext.setCharacterSize(textSz);
    m_Movetext.setPosition(Move.min_x * cellSz + 0.f, Move.min_y * cellSz + 0.f);
    m_Movetext.setFillColor(sf::Color(0,0,0,128));
    m_Movetext.setString("MOVE");

//create setposition resize...
    //m_window.create(sf::VideoMode(1, 1), title, sf::Style::Close);
    unsigned int width = 12 * cellSz;//384;
    unsigned int height = 3 * cellSz;//96;
    m_window.create(sf::VideoMode(width, height), title, sf::Style::Close);
    m_window.setPosition(parent.getPosition() + sf::Vector2i(100,100));
    //m_window.setSize({384,96});
    //sf::View view;
    //view.setCenter(384/2,96/2);
    //view.setSize(384,96);
    //m_window.setView(view);
    
    //run();
}

MSG TextBox::run()
{
    auto repositionText = [this]()->void
    {
        m_cursor.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width, 
            m_text.getPosition().y - 10.f);
        m_text2.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width + m_cursor.getGlobalBounds().width + 2.f, 
            m_text.getPosition().y);
    };


    MSG msg = MSG::CLOSED;
    sf::Event event;
    sf::Clock clock;
    std::string m_string2;
    float tick = 0.5f;
    float ticker = 0.f;
    bool cursorIsShowing = true;
    //bool textEntered = false;

    Coord mouseCoord;
    Button *hoveredButton = nullptr;
    Button *lastHovered = nullptr;
    while (m_window.isOpen())
    {
        float elapsed = clock.restart().asSeconds();
        //draw
        m_window.clear(sf::Color::White);
        m_grid.render(m_window);
        m_window.draw(m_instructionText);
        m_window.draw(m_text);
        if(cursorIsShowing) m_window.draw(m_cursor);
        m_window.draw(m_text2);

        m_window.draw(m_OKtext);
        m_window.draw(m_Canceltext);
        m_window.draw(m_Deletetext);
        m_window.draw(m_Movetext);
        m_window.display();

        while (m_window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                m_window.close();
                break;
            case sf::Event::LostFocus:
                if(parent.hasFocus())
                    m_window.requestFocus();
                break;
            case sf::Event::TextEntered:
            {//set max length???
                if(m_string.size() < 45 && event.text.unicode < 128 && event.text.unicode >=32)
                {
                    m_string += char(event.text.unicode);
                    m_text.setString(m_string);
                }
                else if(event.text.unicode == 8)//backspace
                {
                    if(m_string.size())
                    {
                        m_string.pop_back();
                        m_text.setString(m_string);
                    }
                }
                repositionText();
            }
            break;
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Left)
                {
                    //move left...
                    if(m_string.size())
                    {
                        m_string2.insert(m_string2.begin(), m_string.back());
                        m_string.pop_back();
                    }
                    m_text.setString(m_string);
                    m_text2.setString(m_string2);
                    repositionText();
                }
                if(event.key.code == sf::Keyboard::Right)
                {
                    //move left...
                    if(m_string2.size())
                    {
                        m_string.insert(m_string.end(), m_string2.front());
                        m_string2.erase(m_string2.begin());
                    }
                    m_text.setString(m_string);
                    m_text2.setString(m_string2);
                    repositionText();
                }
                if(event.key.code == sf::Keyboard::Enter)
                {
                    if(m_string.size() || m_string2.size())
                        msg = MSG::TEXT_CHANGED;
                    m_window.close();
                }
                if(event.key.code == sf::Keyboard::Escape)
                {
                    m_window.close();
                }
            }
            break;
            case sf::Event::MouseMoved:
            {
                lastHovered = hoveredButton;
                mouseCoord = m_grid.getCoordinate(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
                if(OK.isHovered(mouseCoord)) hoveredButton = &OK;
                else if(Cancel.isHovered(mouseCoord)) hoveredButton = &Cancel;
                else if(Delete.isHovered(mouseCoord)) hoveredButton = &Delete;
                else if(Move.isHovered(mouseCoord)) hoveredButton = &Move;
                else hoveredButton = nullptr;
            }
            break;
            case sf::Event::MouseLeft:
            {
                hoveredButton = nullptr;
            }
            break;
            case sf::Event::MouseButtonPressed:
            {
                if(hoveredButton)
                {//have to change it in two places,,, not likeing this
                //better way of mapping should exist
                    if(hoveredButton == &OK)
                    {
                        if(m_string.size() || m_string2.size())
                        msg = MSG::TEXT_CHANGED;
//                        msg = MSG::TEXT_CHANGED;
                    }
                    if(hoveredButton == &Delete) msg = MSG::DELETE;
                    if(hoveredButton == &Move) msg = MSG::MOVE;
                    m_window.close();
                }
            }
            break;
            default:
            break;
            }
        }
        parentMSGLoop();

        //timer
        ticker += elapsed;
        if(ticker >= tick)
        {
            ticker = fmodf(ticker, tick);
            cursorIsShowing = !cursorIsShowing;
        }

        //logic
        if(hoveredButton && hoveredButton != lastHovered) hoveredButton->brighten(m_grid);//NAMESPACE COLLISION
        if(lastHovered && hoveredButton != lastHovered) lastHovered->setColor(m_grid);
        
        sf::sleep(sf::milliseconds(1));
    }

    m_string = m_string + m_string2;
    return msg;
}

std::string TextBox::getString()
{
    for(auto &c : m_string) if(c == '^') c = '\n';
    return m_string;
}

void TextBox::parentMSGLoop()
{
    sf::Event event;
    while(parent.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            m_window.close();
            parent.close();
        }
        //or make it request focus even out of event?
        if(event.type == sf::Event::GainedFocus)
        {
            m_window.requestFocus();
        }
    }
}

//a lot is hardcoded in, but that should be fine...
ListBox::ListBox(sf::Window &_parent, const std::string &title, sf::Font &font, std::vector<std::string> &_names)//, const std::string &title)
    : parent(_parent), m_grid(12, _names.size(), cellSz, cellSz), names(_names)//m_grid(1, 12, 480.f, 20.f)
{
    numEntriesShown = 10;//m_grid.getHeight();
    offset = 0;
    activeId = 0;

    float height = 0;
    for(unsigned int i = offset; i < numEntriesShown; ++i)
    {
        if(i == names.size()) break;
        std::string name = names[i];
        //find and replace
        for(auto &c : name) if(c == '\n') c = ' ';
        sf::Text text;
        text.setFont(font);
        text.setString(name);//level.name);
        text.setFillColor(sf::Color::Black);
        text.setPosition(0, height);
        text.setCharacterSize(textSz);
        m_texts.push_back(text);
        height += cellSz;//text.getGlobalBounds().height + 4;
    }

//    unsigned int width = m_grid.getWidth();
    
    m_window.create(sf::VideoMode(m_grid.getSize().x, (float)numEntriesShown * cellSz), title, sf::Style::Close);
    m_window.setPosition(parent.getPosition() + sf::Vector2i(100,100));
    for(uint x = 0; x < m_grid.getWidth(); ++x)
    for(uint y = 0; y < m_grid.getHeight(); ++y)
        m_grid.setCellColor({x,y}, y % 2 ? sf::Color::White : sf::Color(224,224,224));
//    m_grid.clear(sf::Color::White);
    //run();
    //activeCoord = m_grid.getCoordinate(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
    //m_grid.setCellColor(activeCoord, sf::Color(128,200,128));
}

MSG ListBox::run()
{
    MSG msg = MSG::CLOSED;
    //scrollup or down if mouse is sitting at top or bottom coordinate without moving???
    sf::Event event;
    
    while (m_window.isOpen())
    {
        m_window.clear(sf::Color::White);
        m_grid.render(m_window);
        for(auto &text : m_texts) m_window.draw(text);
        m_window.display();

        while (m_window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                m_window.close();
                break;
            case sf::Event::LostFocus:
                if(parent.hasFocus())
                    m_window.requestFocus();
                break;
            case sf::Event::MouseMoved:
            {
                lastCoord = activeCoord;
                activeCoord = m_grid.getCoordinate(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
                if(lastCoord.j != activeCoord.j)
                {
                    for(uint x = 0; x < m_grid.getWidth(); ++x){
                    m_grid.setCellColor({x,activeCoord.j}, sf::Color(128,200,128,128),true);
                    m_grid.setCellColor({x,lastCoord.j}, lastCoord.j % 2 ? sf::Color::White : sf::Color(224,224,224));
                    }
                }
                activeId = activeCoord.j;// + offset;
            }
                break;
            case sf::Event::MouseButtonPressed:
            {
                //assign active ID return?
                if(activeId < names.size())
                    msg = MSG::LIST_ITEM_CHOSE;
                m_window.close();
            }
            case sf::Event::MouseWheelScrolled: //MouseWheelScrollEvent:
            {
                if(event.mouseWheelScroll.delta < 0.f)
                {
                    if((offset + numEntriesShown) < names.size())//m_texts.size())
                    {
                        offset += 1;
                        m_grid.setOffset(0.f, -cellSz * (float)offset);
                    }
                }
                else
                {
                    if(offset != 0)
                    {
                        offset -= 1;
                        m_grid.setOffset(0.f, -cellSz * (float)offset);
                    }
                }
                /////
//                m_texts.clear();
                unsigned int k = 0;
                for(unsigned int i = offset; k < numEntriesShown; ++i)
                {
                    if(i == names.size()) break;
                    std::string name = names[i];
                    //find and replace
                    for(auto &c : name) if(c == '\n') c = ' ';
                    sf::Text &text = m_texts[k++];
                    //text.setFont(font);
                    text.setString(name);//level.name);
                }
                /////
            }
                break;
            default:
            break;
            }
        }
        parentMSGLoop();

        sf::sleep(sf::milliseconds(1));
    }

    return msg;
}

void ListBox::parentMSGLoop()
{
    sf::Event event;
    while(parent.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            m_window.close();
            parent.close();
        }
        //or make it request focus even out of event?
        if(event.type == sf::Event::GainedFocus)
        {
            m_window.requestFocus();
        }
    }
}

}//namespace PGUI