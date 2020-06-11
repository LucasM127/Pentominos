#include "WinState.hpp"
#include <cassert>

WinState::WinState(StateMgr &mgr, Context &context, WINSTATETYPE type)
    : GameState(mgr, context)//, m_type(type)
{   
    sf::Color color(255,255,255, 128);
    
    //set up the shader
    //or get passed it by asset lib
    m_shader.loadFromFile("GaussianBlur.frag", sf::Shader::Fragment);
    m_shader.setUniform("source", sf::Shader::CurrentTexture);
    m_shader.setUniform("offsetFactor", sf::Vector2f(0.005f,0.005f));
{
    m_winText.setString("Congratulations!");
    m_winText.setFont(*context.font);
    m_winText.setCharacterSize(128);
    m_winText.setFillColor(color);
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
    m_continueText.setFillColor(sf::Color(128,128,128,128));
    float textWidth = m_continueText.getGlobalBounds().width;
    float width = window.getSize().x;
    float height = window.getSize().y;
    m_continueText.setPosition((width - textWidth)/2.f, height - 100.f);
}

    window.setTitle("WINSTATE");

    m_renderTexture.create(window.getSize().x, window.getSize().y);
}

void WinState::init()
{
    board = dynamic_cast<GameBoard*>(getPushedState());
    assert(board != nullptr);
    //board->RED();
}
//how to add a bloom effect ???
//drawing the grid onlly...
void WinState::render()//target here ????
{
    //board = dynamic_cast<GameBoard*>(getPushedState());
    //board->RED();
    //draw to texture...
    m_renderTexture.clear(sf::Color::White);
    grid.render(m_renderTexture);//better way???
    m_renderTexture.display();
    //
    sf::Texture texture = m_renderTexture.getTexture();
    sf::Sprite sprite(texture);

    sf::RenderStates states;
    states.shader = &m_shader;

    //window.clear(sf::Color::Yellow);
//    board->render();
    window.draw(sprite, &m_shader);
//    grid.render(window);
    //window.draw(m_winText);
    window.draw(m_continueText);
    //window.draw(sprite, &m_shader);
    //window.display();
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

void WinState::tick()
{
    board->winUpdate();
}

//want to have flashy blocks... how ????
//set the pointer to the gameboard to flashify its lights.
//use the gameboard...