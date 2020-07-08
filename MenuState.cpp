#include "MenuState.hpp"
#include "Level.hpp"

#include "TextBox.hpp"

#include <iostream>

//right click to rename???

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
//should maybe have a default texture in case it takes long to load a texture ?????
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
    : GameState(mgr, context), p_levels(context.levels)
{
    sf::Clock clock;

    window.setTitle("Pentaminos");

    idSelected = -1;
    grid.clear(sf::Color::Black);

    m_icons.resize(p_levels->size() + NUM_ICONS);//re-allocates and throws the textures out of scope if pushback or emplace back
    m_icons[ID_CREATELEVEL].set(Level::m_icons[0], *context.font);
    m_icons[ID_CREATEFOLDER].set(Level::m_icons[2], *context.font);
    m_icons[ID_SWITCHFOLDER].set(Level::m_icons[1], *context.font);
    for(size_t i = 0; i < p_levels->size(); ++i)
    {
        m_icons[i+NUM_ICONS].set((*p_levels)[i], *context.font);
    }
    
    //position
    for(unsigned int i = 0; i < m_icons.size(); ++i)
    {
        float sz = grid.getCellSize().x;
        float x_pos = 2*sz + 3*sz * (i%7);
        float y_pos = 2*sz + 3*sz * (i/7);
        m_icons[i].sprite.setPosition(x_pos, y_pos);
        m_icons[i].text.setPosition(x_pos, y_pos + sz/2.f);
    }

    {
        m_folderText.setFont(*context.font);
        m_folderText.setCharacterSize((unsigned int)grid.getCellSize().y);
        m_folderText.setFillColor(sf::Color::White);
        m_folderText.setString(context.levelFileName);//"Default Levels");
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
                if(idSelected == ID_CREATELEVEL || idSelected == ID_CREATEFOLDER)
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
            if(idSelected < NUM_ICONS)//add or remove???
            {
                if(idSelected == ID_SWITCHFOLDER)
                {
                    PGUI::ListBox listbox(window, "List contents", font, *p_levels, "SDLFJKKL");
                    PGUI::MSG ret = listbox.run();
                    if(ret == PGUI::MSG::LIST_ITEM_CHOSE)
                    {
                        std::cout<<"Item chosen was: "<<(*p_levels)[listbox.getChosenId()].name<<std::endl;
                    }
                }
                if(idSelected == ID_CREATELEVEL)
                    requestStateChange(PLAYGROUND);
                if(idSelected == ID_CREATEFOLDER)
                {
                    PGUI::TextBox textbox(window, L"Enter Folder Name", "", font);
                    PGUI::MSG ret = textbox.run();
                }
            }
            else
            {
                if(event.mouseButton.button == sf::Mouse::Right)
                {
                    PGUI::TextBox textbox(window, L"Rename level", (*p_levels)[idSelected-NUM_ICONS].name, font);//, [&]()->void{name = textbox.getString();});
                    PGUI::MSG ret = textbox.run();
                    //?
                    if(ret == PGUI::MSG::TEXT_CHANGED)
                    {
                        (*p_levels)[idSelected-NUM_ICONS].name = textbox.getString();
                        saveLevels(*p_levels, m_folderText.getString());
                    }
                    if(ret == PGUI::MSG::DELETE)
                    {
                        p_levels->erase(p_levels->begin() + (idSelected-NUM_ICONS));
                        saveLevels(*p_levels, m_folderText.getString());
                    }
                }
                else
                requestStateChange(PLAY, idSelected - NUM_ICONS);
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