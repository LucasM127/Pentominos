#include "WinState.hpp"
#include "Colors.hpp"
#include <cassert>

WinState::WinState(StateMgr &mgr, Context &context, WINSTATETYPE type)
    : GameState(mgr, context), board(*context.board)
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
}

//won't need this then
void WinState::init()
{
    //board = dynamic_cast<GameBoard*>(getPushedState());
    //assert(board != nullptr);
}

void WinState::render()//target here ????
{
    //draw to texture for bloom
    m_renderTexture.clear(sf::Color::White);
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

void draw(Coord C, GameBoard &board, Grid &grid)
{
    const std::vector<Coord> &changedCoords = board.getChangedCoords();
    const std::vector<uint8_t> &data = board.getData();
    const std::vector<Pentamino> &blocks = board.getBlocks();
    const std::vector<float> &blockHues = board.getBlockHues();
    Pentamino *p_selectedBlock = board.getSelectedBlock();
    CoordMapper &CM = board.CM;

    if(!CM.isValid(C)) return;
    unsigned int id = data[CM.mapID(C)];
    bool inWinShape = board.isInWinShape(C);

    sf::Color color;

    if(inWinShape)
    {
        if(board.isValid()) color = sf::Color::White;
        else color = sf::Color(255,170,170);
    }
    else color = sf::Color::Black;
    grid.setCellColor(C, color, false);

    if( (id&0x0F) > 12)
    {
        if(id == BACKGROUND_ID) grid.setCellTexture(C, {256.f,128.f}, {128.f, 128.f});
        else if(id == WINZONE_ID) grid.setCellTexture(C, {384.f,0.f}, {128.f, 128.f});
        else throw;
        return;
    }
    
    //a piece is on the board at this location
    int i = (id&0xF0)>>4;//16 ???
    //assert(i < 5);
    id = id&0x0F;
    
    int texId = blocks[id].texIDs[i];
    int orientation = blocks[id].orientations[i];
    bool flipped = blocks[id].isFlipped;

    int x_off = texId%4;
    int y_off = texId/4;
    sf::Vector2f uvpos(128.f * (float)x_off, 128.f * (float)y_off);
    grid.setCellTexture(C, uvpos, {128.f, 128.f}, orientation, flipped);

    //color of the piece at the location
    HSL hsl;
    hsl.hue = blockHues[id];
    hsl.lightness = 0.4f;
    //float luminance = 0.4f;

    if(inWinShape)
    {
        hsl.saturation = 0.7f;
    }
    else hsl.saturation = 0.1f;

    //is a playground ?

    if(board.isWon())// || isPlayground)
    {
        hsl.saturation = 0.9f;//*= 1.35f;
        hsl.lightness = 0.6f;//1.f;
        //luminance = 0.6f;
    }

    if(id == board.getHoverId() && p_selectedBlock == nullptr)
    {
        hsl.saturation += 0.1f;
        hsl.lightness += 0.1f;
        //luminance += 0.1f;
    }

    color = fromHSL(hsl);
    color.a = board.blockAlpha();
    //color = setLuminance(color, luminance);
    grid.setCellColor(C, color, true);
}

void WinState::tick()
{
    board.winUpdate();
    for(auto &C : board.getChangedCoords())
        draw(C, board, grid);
}