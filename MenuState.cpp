#include "MenuState.hpp"
#include "Level.hpp"

#include <iostream>

//!!!
const uint INVALID_ID = -1;

MenuState::MenuState(StateMgr &mgr, Context &context)
    : GameState(mgr, context), font(*context.font)
{
    idSelected = -1;
    grid.clear(sf::Color::Black);
    //for each level in levels!
    float maxTextWidth = 0.f;
    float textHeight = 0.f;
    for(auto &lvl : Level::m_preLoadedLevels)
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(32);
        text.setFillColor(sf::Color::White);
        text.setString(lvl.name);
        float width = text.getLocalBounds().width;
        float height = text.getLocalBounds().height;
        textHeight += height;
        if(width > maxTextWidth) maxTextWidth = width;
        m_levelTexts.push_back(text);
    }
    /*
    //EDit mode!
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(32);
        text.setFillColor(sf::Color::White);
        text.setString("Edit Mode");
        float width = text.getLocalBounds().width;
        float height = text.getLocalBounds().height;
        textHeight += height;
        if(width > maxTextWidth) maxTextWidth = width;
        m_levelTexts.push_back(text);
    }*/

    //position on screen...
    float winWidth = window.getSize().x;
    float winHeight = window.getSize().y;
    float x_offset = (winWidth - maxTextWidth) / 2.f;
    float y_offset = (winHeight - textHeight) / 2.f;
    textHeight = y_offset;
    for(auto &text : m_levelTexts)
    {
        text.setPosition(x_offset, textHeight);
        textHeight += text.getLocalBounds().height;
    }

    std::cout<<m_levelTexts.size()<<" levles size "<<Level::m_preLoadedLevels.size()<<std::endl;
}

void MenuState::handleEvent(const sf::Event &event)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if(event.type == sf::Event::MouseMoved)
    {
        idLastSelected = idSelected;
        idSelected = INVALID_ID;
        for(uint i = 0; i < m_levelTexts.size(); i++)
        {
            auto &text = m_levelTexts[i];
            if(text.getGlobalBounds().contains(mousePos))
            {
                idSelected = i;
                break;
            }
        }
        if(idLastSelected != idSelected)
        {
            if(idSelected != INVALID_ID) m_levelTexts[idSelected].setFillColor(sf::Color::Yellow);
            if(idLastSelected != INVALID_ID) m_levelTexts[idLastSelected].setFillColor(sf::Color::White);
        }
    }

    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(idSelected != INVALID_ID)
        {
            if(idSelected < m_levelTexts.size())
                requestStateChange(PLAY, idSelected);//:(
            /*else
            {
                requestStateChange(EDIT);
            }*/
            
        }
    }
}

void MenuState::render()
{
    //window.clear(sf::Color::Black);
    grid.render(window);
    for(auto &text : m_levelTexts) window.draw(text);
    //window.display();
}