#include "WinState.hpp"
#include "Colors.hpp"
#include <cassert>

WinState::WinState(StateMgr &mgr, Context &context, WINSTATETYPE type)
    : GameState(mgr, context), board(*context.board), m_controller(board, grid)
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
    m_continueText.setString("Press [SPACE] to continue");
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

    CoordMapper &CM = board.CM;
    for(uint w = 0; w<CM.width; w++)
        for(uint h = 0; h<CM.height;h++)
        {
            m_drawSettings.draw({w,h}, grid, board);
        }
    
    m_activeId= 0;
}

void WinState::render()//target here ????
{
    //draw to texture for bloom
    m_renderTexture.clear(sf::Color::Black);
    grid.render(m_renderTexture);//better way???
    m_renderTexture.display();
    
    sf::Texture texture = m_renderTexture.getTexture();
    sf::Sprite sprite(texture);

    sf::RenderStates states;
    states.shader = &m_shader;

    window.draw(sprite, &m_shader);
    //window.draw(m_winText);
    window.draw(m_continueText);
}

void WinState::handleEvent(const sf::Event &event)
{

    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
    {
        requestStateChange(MENU);
    }
}

void WinState::tick()
{
    //change the id... and ...
    unsigned int lastId = m_activeId;
    m_activeId = (m_activeId+1)%board.m_blocks.size();
    //redraw the 2 blocks
    for(auto &_C : board.m_blocks[lastId].m_coords)
    {
        Coord C = board.m_blocks[lastId].m_pos + _C;
        m_drawSettings.draw(C, grid, board, m_activeId);
    }
    for(auto &_C : board.m_blocks[m_activeId].m_coords)
    {
        Coord C = board.m_blocks[m_activeId].m_pos + _C;
        m_drawSettings.draw(C, grid, board, m_activeId);
    }
}