#include "MenuState.hpp"
#include "Level.hpp"

#include "TextBox.hpp"

#include <iostream>

void MenuState::Icon::set(const Level &level, sf::Font &font)
{
    sf::Image image;
    image.create(24,24, sf::Color(0,0,0,0));
    uint x_off = (24 - level.width) / 2;
    uint y_off = (24 - level.height) / 2;

    for(unsigned int y = 0; y < level.height; ++y)
    {
        uint32_t data = reverseBits(level.data[y], level.width);
        for(unsigned int x = 0; x < level.width; ++x)
        {
            if(checkBit(data, x))
                image.setPixel(x + x_off, y + y_off, sf::Color(255,255,255,192));
        }
    }

    texture.loadFromImage(image);

    sprite.setTexture(texture);
    sprite.setOrigin(12.f,12.f);
    sprite.scale(128.f/24.f,128.f/24.f);

    //only some way to set 'centered' multiline
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setString(level.name);
    text.setStyle(sf::Text::Underlined);
    float width = text.getLocalBounds().width;
    text.setOrigin(width/2,0);
}

bool MenuState::Icon::isHovered(const sf::Vector2f &mousePos)
{
    return sprite.getGlobalBounds().contains(mousePos);
}

MenuState::MenuState(StateMgr &mgr, Context &context)
    : GameState(mgr, context), levels(*context.levels), ID_CREATE(0), ID_SWITCHFOLDER(1)
{
    sf::Clock clock;

    window.setTitle("Pentaminos");

    idSelected = -1;
    grid.clear(sf::Color::Black);

    levels = Level::m_preLoadedLevels;

    m_icons.resize(levels.size());//re-allocates and throws the textures out of scope if pushback or emplace back
    for(size_t i = 0; i < levels.size(); ++i)
    {
        m_icons[i].set(levels[i], *context.font);
    }
    
    //position
    for(unsigned int i = 0; i < m_icons.size(); ++i)
    {
        float sz = grid.getCellSize();
        float x_pos = 2*sz + 3*sz * (i%7);
        float y_pos = 2*sz + 3*sz * (i/7);
        m_icons[i].sprite.setPosition(x_pos, y_pos);
        m_icons[i].text.setPosition(x_pos, y_pos + sz/2.f);
    }

    {
        m_folderText.setFont(*context.font);
        m_folderText.setCharacterSize((unsigned int)grid.getCellSize());
        m_folderText.setFillColor(sf::Color::White);
        m_folderText.setString("Default Levels");
        float width = m_folderText.getLocalBounds().width;
        m_folderText.setOrigin(width/2.f,0.f);
        m_folderText.setPosition(grid.getSize().x/2.f + grid.getOffset().x,0.f);
    }

    std::cout<<"Loading menu took "<<clock.restart().asMilliseconds()<<" ms\n";
}

void MenuState::handleEvent(const sf::Event &event)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if(event.type == sf::Event::MouseMoved)
    {
        idLastSelected = idSelected;
        idSelected = INVALID_ID;
        
        for(uint i = 0; i < m_icons.size(); i++)
        {
            auto &icon = m_icons[i];
            if(icon.isHovered(mousePos))
            {
                idSelected = i;
                break;
            }
        }
        if(idLastSelected != idSelected)
        {
            if(idSelected != INVALID_ID)
            {
                if(idSelected == ID_CREATE)
                    m_icons[idSelected].sprite.setColor(sf::Color::Red);
                else if(idSelected == ID_SWITCHFOLDER)
                    m_icons[idSelected].sprite.setColor(sf::Color::Yellow);
                else
                    m_icons[idSelected].sprite.setColor(sf::Color::Green);
                m_icons[idSelected].sprite.scale(1.25f,1.25f);
            }
            if(idLastSelected != INVALID_ID)// m_levelTexts[idLastSelected].setFillColor(sf::Color::White);
            {
                m_icons[idLastSelected].sprite.setColor(sf::Color::White);
                m_icons[idLastSelected].sprite.scale(0.8f,0.8f);
            }
        }
    }

    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(idSelected != INVALID_ID)
        {
            if(idSelected < 2)//add or remove???
            {
                if(idSelected == ID_SWITCHFOLDER);
                if(idSelected == ID_CREATE)
                    PGUI::TextBox textbox(window, L"Enter Level Name...", font, [](){;});
                requestStateChange(PLAYGROUND);
            }
            else
            {
                requestStateChange(PLAY, idSelected);
            }
            
        }
    }

}

void MenuState::render()
{
    grid.render(window);
    window.draw(m_folderText);
    for(auto &icon : m_icons) window.draw(icon.sprite);
    if(idSelected != INVALID_ID)
        window.draw(m_icons[idSelected].text);
}