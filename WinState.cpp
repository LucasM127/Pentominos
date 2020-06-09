#include "WinState.hpp"

WinState::WinState(StateMgr &mgr, Context &context, WINSTATETYPE type)
    : GameState(mgr, context), m_type(type)
{
    /*
    //set up the shader
    //or get passed it by asset lib
    m_shader.loadFromFile("GaussianBlur.frag", sf::Shader::Fragment);
    m_shader.setUniform("source", sf::Shader::CurrentTexture);
    m_shader.setUniform("offsetFactor", sf::Vector2f(0.005f,0.005f));*/
{
    m_winText.setString("Congratulations!");
    m_winText.setFont(*context.font);
    m_winText.setCharacterSize(128);
    m_winText.setFillColor(sf::Color::White);
    float textWidth = m_winText.getGlobalBounds().width;
    float width = window.getSize().x;
    float textHeight = m_winText.getGlobalBounds().height;
    float height = window.getSize().y;
    m_winText.setPosition((width - textWidth)/2.f, (height - textHeight)/2.f);
}

{
    m_continueText.setString("Press SPACE to continue");
    m_continueText.setFont(*context.font);
    m_continueText.setCharacterSize(32);
    m_continueText.setFillColor(sf::Color::White);
    float textWidth = m_continueText.getGlobalBounds().width;
    float width = window.getSize().x;
    
    m_continueText.setPosition((width - textWidth)/2.f, m_winText.getPosition().y + 200.f);
}

    window.setTitle("WINSTATE");

    m_renderTexture.create(window.getSize().x, window.getSize().y);
}

void WinState::render()
{
    //draw to texture...
    //m_renderTexture.clear(sf::Color::White);
    //grid.render(m_renderTexture);//better way???
    //m_renderTexture.display();
    //
    //sf::Texture texture = m_renderTexture.getTexture();
    //sf::Sprite sprite(texture);

//    sf::RenderStates states;
//    states.shader = &m_shader;

    window.clear(sf::Color::Yellow);
    grid.render(window);
    window.draw(m_winText);
    window.draw(m_continueText);
    //window.draw(sprite, &m_shader);
    window.display();
}//just need a way to revert to last state or not ???

void WinState::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        //requestStateChange(PLAY, pState);
        //depends on if is WON or PAUSED...
    {
        //if(m_type == WINSTATETYPE::WON)
            //requestStatePop();//or should i clear it and then push a new level...
            requestStateChange(MENU);
    }
}