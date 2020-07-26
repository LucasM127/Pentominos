#include "HelpState.hpp"

HelpState::HelpState(StateMgr &mgr, Context &context) : GameState(mgr, context)
{
    window.setTitle("Help Screen");

    paint();

    float cellSz = grid.getCellSize().y;
    //load the help icon too and display it on the right of the grid... ???
    //use same function for texture drawing
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setString("[Left Click]");
        text.setPosition(3*cellSz,cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color(128,128,128,128));
        text.setStyle(sf::Text::Italic);
        text.setString("Select or place piece");
        text.setPosition(5*cellSz,2*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setString("[Right Click]");
        text.setPosition(3*cellSz,3*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color(128,128,128,128));
        text.setStyle(sf::Text::Italic);
        text.setString("Drop piece");
        text.setPosition(5*cellSz,4*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setString("[A] and [D] keys");
        text.setPosition(3*cellSz,5*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color(128,128,128,128));
        text.setStyle(sf::Text::Italic);
        text.setString("Rotate piece");
        text.setPosition(5*cellSz,6*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setString("[W] key");
        text.setPosition(3*cellSz,7*cellSz);
        m_texts.push_back(text);
    }
    {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(cellSz * 0.75f);
        text.setFillColor(sf::Color(128,128,128,128));
        text.setStyle(sf::Text::Italic);
        text.setString("Flip piece");
        text.setPosition(5*cellSz,8*cellSz);
        m_texts.push_back(text);
    }
    setBottomText("Press any key to continue");

    position();
}

//this is no good again... :()
void HelpState::position()
{
    float cellSz = grid.getCellSize().y;
    
    for(uint i = 0; i < m_texts.size(); ++i)
    {
        //hmm
        m_texts[i].setCharacterSize(uint(cellSz * 0.75f));
        if(i%2)
            m_texts[i].setPosition(5.f * cellSz + grid.getOffset().x + m_viewRect.P.i * cellSz, float(i + 1) * cellSz + grid.getOffset().y + m_viewRect.P.j * cellSz);
        else
            m_texts[i].setPosition(3.f * cellSz + grid.getOffset().x + m_viewRect.P.i * cellSz, float(i + 1) * cellSz + grid.getOffset().y + m_viewRect.P.j * cellSz); 
    }
}

void HelpState::paint()
{
    const Level &level = Level::m_icons[3];//the help icon
    
    uint height = m_viewRect.height;//grid.getHeight();
    uint width = m_viewRect.width;//grid.getWidth();

    //assert(width > level.width + 1);
    uint x_offset = (width - level.width) - 1;//on the right
    uint y_offset = (height - level.height)/2;

    for(uint i =0; i< level.width; i++)
        for(uint j= 0; j<level.height; j++)
    {
        uint32_t data = reverseBits(level.data[j], level.width);
        if(checkBit(data, i))//  m_data[C.j], C.i))
        {
            Coord C(i + x_offset, j + y_offset);
            Coord C_Grid = m_viewRect.transform(C);
                grid.setCellColor(C_Grid, sf::Color(64,64,64,160));
                //grid.setCellColor(C, sf::Color(255,64,64));//(255 * C.i / 22,255 * C.j / 13,128));//sf::Color::White);
                grid.setCellTexture(C_Grid,getTextureUV(TexAtlasID::WINZONE_TEXTURE),getTextureSize());
        }
    }

}

void HelpState::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
        requestStateChange(MENU);
    if(event.type == sf::Event::Resized)
        position();
}

void HelpState::render()
{
    grid.render(window);
    for(auto &text : m_texts) window.draw(text);
    window.draw(m_bottomText);
    //window.draw(sprite);
//    window.draw(m_instructionText);
}