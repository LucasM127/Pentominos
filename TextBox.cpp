#include "TextBox.hpp"

#include <cmath> //fmodf

namespace PGUI
{

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

TextBox::TextBox(sf::Window &_parent, const std::wstring & title, sf::Font &font, std::function<void()> fn)
    : parent(_parent), m_function(fn), m_grid(12,3,40.f)
{
    m_text.setFont(font);
    m_text.setCharacterSize(24);
    m_text.setPosition(0.f, 40.f);
    m_text.setFillColor(sf::Color::Black);

    m_cursor.setFont(font);
    m_cursor.setString("|");
    m_cursor.setCharacterSize(40);
    m_cursor.setFillColor(sf::Color(255,255,255,128));
    m_cursor.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width, 
                         m_text.getPosition().y - 5.f);

    m_grid.clear(sf::Color::White);
    for(unsigned int x = 0; x < m_grid.getWidth(); ++x)
        m_grid.setCellColor({x,1},sf::Color(192,192,192));
    OK.set(6,8,2,2);
    OK.color = COLOR::brighten(sf::Color::Green, 0.5f);
    OK.setColor(m_grid);
    Cancel.set(9,11,2,2);
    Cancel.color = COLOR::brighten(sf::Color::Red, 0.5f);
    Cancel.setColor(m_grid);

    m_OKtext.setFont(font);
    m_OKtext.setCharacterSize(24);
    m_OKtext.setPosition(OK.min_x * 40.f + 15.f, OK.min_y * 40.f + 5.f);
    m_OKtext.setFillColor(sf::Color(0,0,0,128));
    m_OKtext.setString("OK");

    m_Canceltext.setFont(font);
    m_Canceltext.setCharacterSize(24);
    m_Canceltext.setPosition(Cancel.min_x * 40.f + 15.f, Cancel.min_y * 40.f + 5.f);
    m_Canceltext.setFillColor(sf::Color(0,0,0,128));
    m_Canceltext.setString("CANCEL");

    m_window.create(sf::VideoMode(480, 120), title, sf::Style::Close);
    m_window.setPosition(parent.getPosition() + sf::Vector2i(100,100));
    
    run();
}

void TextBox::run()
{
    sf::Event event;
    sf::Clock clock;
    float tick = 0.5f;
    float ticker = 0.f;
    bool cursorIsShowing = true;

    Coord mouseCoord;
    Button *hoveredButton = nullptr;
    Button *lastHovered = nullptr;
    while (m_window.isOpen())
    {
        float elapsed = clock.restart().asSeconds();
        //draw
        m_window.clear(sf::Color::White);
        m_grid.render(m_window);
        m_window.draw(m_text);
        if(cursorIsShowing) m_window.draw(m_cursor);

        m_window.draw(m_OKtext);
        m_window.draw(m_Canceltext);
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
                m_cursor.setPosition(m_text.getPosition().x + m_text.getGlobalBounds().width, 
                                     m_text.getPosition().y - 10.f);
            }
            break;
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Enter)
                {
                    m_function();
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
                else hoveredButton = nullptr;
            }
            break;
            case sf::Event::MouseButtonPressed:
            {
                if(hoveredButton)
                {
                    if(hoveredButton == &OK) m_function();
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

}//namespace PGUI