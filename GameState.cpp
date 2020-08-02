#include "GameState.hpp"
#include "WelcomeState.hpp"
#include "GameBoard.hpp"
#include "WinState.hpp"
#include "MenuState.hpp"
#include "PlayState.hpp"
#include "PlayGroundState.hpp"
#include "HelpState.hpp"
#include <iostream>
#include <cassert>

//Can think of welcome screen as a winzone map... set width/height/ same save file, with a title.
//use a custom draw function for the textures is all!
//then can center it on the screen better
//16x9
//1024 x 768
//goes weird if width ever exceeds 32 lol -> cause of winShape as uint32_t
//64.f size built

//Only thing.. minimal grid cell size!
StateMgr::StateMgr() : amFullScreen(false)// : m_grid()//22,14,64.f,64.f)//m_grid(32,18,50.f)//m_grid(24,32,32)// 
{
    m_curGameState = nullptr;//???
    pOldState = nullptr;

//hope it won't throw...!
    m_context.window = &m_window;
    m_context.grid = &m_grid;
    m_context.board = &m_board;
    m_context.font = &m_font;
    //m_context.texture = &m_texture;
    m_context.borderColor = sf::Color(96,64,64);//::Cyan;
    m_context.borderTexID = TexAtlasID::INSIDE_PIECE;

    m_context.activeFolder = new Folder("Default");

    //load assets
    //TODO: TEST FOR LOADFAIL... what to do... abstract out?
    m_texture.loadFromFile("Assets/TexAtlas.png");//Elephant.png");//
    m_texture.setSmooth(true);
    m_grid.setTexMap(&m_texture);
    m_font.loadFromFile("Assets/FreeSans.ttf");

    //FULLSCREEN or custom window size...
    //calculate grid size based off of window size....
    {
        uint width = 48u * BOARD_WIDTH;
        uint height = 48u * BOARD_HEIGHT;
        //m_window.create(modes[0], "Pentominos", sf::Style::Fullscreen);
        m_window.create(sf::VideoMode(width, height), "Pentominos");
/*
        float boardAspectRatio = float(BOARD_WIDTH) / float(BOARD_HEIGHT);
        unsigned int winWidth  = m_window.getSize().x;
        unsigned int winHeight = m_window.getSize().y;

        float winAspectRatio = float(winWidth) / float(winHeight);

        unsigned int cellSz;
        if(winAspectRatio > boardAspectRatio)//the window is 'wider' than taller, black bars on left and right
            cellSz = float(winHeight) / float(BOARD_HEIGHT);
        else
            cellSz = float(winWidth) / float(BOARD_WIDTH);
            
        unsigned int x_offset = (winWidth - (cellSz * BOARD_WIDTH)) / 2;
        unsigned int y_offset = (winHeight - (cellSz * BOARD_HEIGHT)) / 2;
        
        m_grid.setOffset(float(x_offset), float(y_offset));

        m_grid.resize(BOARD_WIDTH, BOARD_HEIGHT, float(cellSz), float(cellSz));

        PGUI::setCellSize(float(cellSz/2u));*/
        resize(width, height);
    }

    //set the window and grid sizes here... common to all states.
    //float width = m_grid.getSize().x;
    //float height = m_grid.getSize().y;

//    m_grid.setOffset((1920-width)/2,(1080-height)/2);
/*
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    for (std::size_t i = 0; i < modes.size(); ++i)
{
    sf::VideoMode mode = modes[i];
    std::cout << "Mode #" << i << ": "
              << mode.width << "x" << mode.height << " - "
              << mode.bitsPerPixel << " bpp" << std::endl;
}
//modes[10]*/
    //m_window.create(sf::VideoMode(1920,1080), "Pentominos", sf::Style::Fullscreen);
    //position it somewhere else???
    //with x11???
    //::Display *display;
    //::Window win = XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)), 0,0, 0, 0, 0, 0,0);
    //HACK to prevent 'jump'
    //m_window.create(sf::VideoMode(1, 1), "Pentominos", sf::Style::Close);
    //m_window.create(sf::VideoMode(1920, 1080), "Pentominos", sf::Style::Fullscreen);
    //m_window.create(sf::VideoMode(width, height), "Pentominos", sf::Style::Close);
    //m_window.setPosition({1920+(1920 - (int)width)/2 , (1080 - (int)height)/2});//hmmmm
    //m_window.setSize({(uint)width,(uint)height});//int -> unsigned int
    //sf::View view;
    //view.setCenter(width/2, height/2);
    //view.setSize(width, height);
    //m_window.setView(view);

    m_stateMapping[WELCOME] = [this](int x)->GameState*{return new WelcomeState(*this, m_context);};
    m_stateMapping[MENU] = [this](int x)->GameState*{return new MenuState(*this, m_context);};
    m_stateMapping[PLAY] = [this](int x)->GameState*{return new PlayState(*this, m_context, x);};//where x = level
    //m_stateMapping[EDIT] = [this](int x)->GameState*{return new EditBoard(*this, m_context);};
    m_stateMapping[PLAYGROUND] = [this](int x)->GameState*{return new PlayGroundState(*this, m_context);};
    m_stateMapping[WIN] = [this] (int x)->GameState*{return new WinState(*this, m_context, (WINSTATETYPE)x);};//How to send additional DATA?
    m_stateMapping[HELP] = [this](int x)->GameState*{return new HelpState(*this, m_context);};

    sf::Image image;
    uint8_t colors[16] = {
        0x00,0x00,0xFF,0x80,//green
        0xFF,0x00,0x00,0x80,
        0x00,0xFF,0x00,0x80,
        0xFF,0xFF,0x00,0x80};//yellow
    image.create(2,2,colors);//rgba
    m_bgTexture.loadFromImage(image);
    m_bgTexture.setSmooth(true);
    m_bgSprite.setTexture(m_bgTexture);
    m_bgSprite.setScale((float)m_window.getSize().x/2.f, (float)m_window.getSize().y/2.f);

    newStateWasRequested = false;// true;
    m_curGameState = m_stateMapping[WELCOME](0);
    //nextState = WELCOME;
    //action = REPLACE;
}

StateMgr::~StateMgr()
{
    delete m_curGameState;
    if(pOldState) delete pOldState;
    delete m_context.activeFolder;
}

void StateMgr::resize(uint winWidth, uint winHeight)
{
    unsigned int minCellSz = 16u;//minimal cell size!

    m_window.setView(sf::View(sf::FloatRect(0,0,float(winWidth), float(winHeight))));

    float boardAspectRatio = float(BOARD_WIDTH) / float(BOARD_HEIGHT);

    float winAspectRatio = float(winWidth) / float(winHeight);

    unsigned int cellSz;
    if(winAspectRatio > boardAspectRatio)//the window is 'wider' than taller, black bars on left and right
        cellSz = float(winHeight) / float(BOARD_HEIGHT);
    else
        cellSz = float(winWidth) / float(BOARD_WIDTH);
    
    //TODO FIX ME
    //resize a window so no smaller than a minimal size?
    if(cellSz < minCellSz)
    {
        //reset window to its last size!
        if(winAspectRatio > boardAspectRatio)//the window is 'wider' than taller, black bars on left and right
            m_window.setSize({winWidth, minCellSz * BOARD_HEIGHT});
        else
            m_window.setSize({minCellSz * BOARD_WIDTH , winHeight});
        return;
        //resize(m_window.getSize().x, m_window.getSize().y);
    }
    
    //number of pixels to offset the grid surface to be fully visible at the aspect ratio
    unsigned int x_offset = (winWidth - (cellSz * BOARD_WIDTH)) / 2;
    unsigned int y_offset = (winHeight - (cellSz * BOARD_HEIGHT)) / 2;

    //ASSERT CELLSZ > 0
    //size of grid 'border', may/may not be clipped, not part of display surface
    unsigned int x_padding = (x_offset / cellSz) + 1;
    unsigned int y_padding = (y_offset / cellSz) + 1;
    
    //pixel coordinates to position the grid to fit properly accounting for padding
    //in view space so negative, float.
    float grid_offset_x = float(x_offset) - float(x_padding) * float(cellSz);
    float grid_offset_y = float(y_offset) - float(y_padding) * float(cellSz);
    
    m_grid.setOffset(grid_offset_x, grid_offset_y);

    m_grid.resize(BOARD_WIDTH + 2 * x_padding, BOARD_HEIGHT + 2 * y_padding, float(cellSz), float(cellSz));

    PGUI::setCellSize(float(cellSz/2u));

    m_bgSprite.setScale(float(winWidth)/2.f, float(winHeight)/2.f);

    if(m_curGameState) m_curGameState->setViewRect(x_padding, y_padding, BOARD_WIDTH, BOARD_HEIGHT);
    m_context.viewRect.set(x_padding, y_padding, BOARD_WIDTH, BOARD_HEIGHT);

    //repaint!
    //Here is where we have the ...
    //default should be a property of the state, global, like the other shits?
    for(unsigned int i = 0; i < x_padding; ++i)
        for(unsigned int j = 0; j < m_grid.getHeight(); ++j)
        {
            m_grid.setCellColor({i,j}, m_context.borderColor);
            m_grid.setCellTexture({i,j}, getTextureUV(m_context.borderTexID), getTextureSize());
        }
    for(unsigned int i = x_padding; i < m_grid.getWidth() - x_padding; ++i)
        for(unsigned int j = 0; j < y_padding; ++j)
        {
            m_grid.setCellColor({i,j}, m_context.borderColor);
            m_grid.setCellTexture({i,j}, getTextureUV(m_context.borderTexID), getTextureSize());
        }
    for(unsigned int i = x_padding; i < m_grid.getWidth() - x_padding; ++i)
        for(unsigned int j = BOARD_HEIGHT + y_padding; j < m_grid.getHeight(); ++j)
        {
            m_grid.setCellColor({i,j}, m_context.borderColor);
            m_grid.setCellTexture({i,j}, getTextureUV(m_context.borderTexID), getTextureSize());
        }
    for(unsigned int i = BOARD_WIDTH + x_padding; i < m_grid.getWidth(); ++i)
        for(unsigned int j = 0; j < m_grid.getHeight(); ++j)
        {
            m_grid.setCellColor({i,j}, m_context.borderColor);
            m_grid.setCellTexture({i,j}, getTextureUV(m_context.borderTexID), getTextureSize());
        }
    
    if(m_curGameState) m_curGameState->onPaint();
}

void StateMgr::run()
{
    sf::Event event;
    sf::Clock clock;
    float elapsedTime = 0.f;
    float tickTime = 0.25f;//0.03125f;
    float refreshTime = 1.f / 60.f;
    float tickAccumulator = 0.f;
    float renderAccumulator = 0.f;
    float fpsAccumulator = 0.f;
    int numFrames = 0;
    
    //fps display here!
    sf::Text fpsText;
    fpsText.setFont(m_font);
    fpsText.setPosition(m_window.getSize().x - 100.f, 10.f);
    fpsText.setFillColor(sf::Color::Cyan);
    //fpsText.setCharacterSize(30);

    clock.restart();
    
    while(m_window.isOpen())
    {
        //check for requested state change ?
        if(newStateWasRequested)
        {//otherwise just hold on to it ???
            GameState *pNextState;
            if(action == REPLACE || action == PUSH)
                pNextState = m_stateMapping[nextState](stateArg);//where do I get the arguments? from request stateChange(int)

            switch (action)
            {
            case PUSH:
                pOldState = m_curGameState;
                m_curGameState = pNextState;//if set
                break;
            case POP:
                delete m_curGameState;
                m_curGameState = pOldState;
                pOldState = nullptr;
                break;
            case REPLACE://clears....
                if(pOldState) delete pOldState;
                pOldState = nullptr;
                delete m_curGameState;
                m_curGameState = pNextState;
                break;
            default:
                break;
            }
    
            newStateWasRequested = false;//delete it???
            //do
            m_curGameState->init();//then it works.... and can ask for the pushed pointer here
        }

        elapsedTime = clock.restart().asSeconds();
        
        tickAccumulator += elapsedTime;
        renderAccumulator += elapsedTime;
        fpsAccumulator += elapsedTime;

//        m_window.waitEvent(event);
        while (m_window.pollEvent(event))
        {
            if(event.type == sf::Event::Resized)
            {
                //resize the grid...
                resize(event.size.width, event.size.height);
            }
            m_curGameState->onEvent(event);//handleEvent(event);
            if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape)
                    m_window.close();
                if(event.key.code == sf::Keyboard::F)
                {
                    //toggle fullscreen...
                    //not posting a resize event in windows :? should be on creation...
                    if(!amFullScreen)
                    {
                        lastWinSz = m_window.getSize();
                        sf::VideoMode mode = sf::VideoMode::getDesktopMode();
                        m_window.create(mode, "Pentominos", sf::Style::Fullscreen);
                        resize(mode.width, mode.height);
                    }
                    else
                    {
                        m_window.create(sf::VideoMode(lastWinSz.x, lastWinSz.y), "Pentominos");
                        resize(lastWinSz.x, lastWinSz.y);
                    }
                    //tell the GameStates that a resize event occured, recreating a fullscreen window doesn't trigger a resize event
                    sf::Event e;
                    e.type = sf::Event::Resized;
                    e.size.width = m_window.getSize().x;
                    e.size.height = m_window.getSize().y;
                    m_curGameState->onEvent(e);

                    amFullScreen = !amFullScreen;
                }
            }
            if(event.type == sf::Event::Closed) m_window.close();

            m_curGameState->update();//updates with the events as a board game
        }
        //tick()
        if(tickAccumulator >= tickTime)
        {
            while(tickAccumulator >= tickTime) tickAccumulator -= tickTime;
            m_curGameState->tick();
            //numFrames++;
        }

        if(fpsAccumulator >= 1.f)
        {
            while(fpsAccumulator > 1.f) fpsAccumulator -= 1.f;//unless there is a real big lag spike!
            fpsText.setString(std::to_string(numFrames));
            numFrames = 0;
        }

//i bet it is because i have update logic in render ---> yup
        if(renderAccumulator >= refreshTime)
        {
            while(renderAccumulator >= refreshTime) renderAccumulator -= refreshTime;
            m_window.clear(sf::Color::Black);
            //draw a background effect for the splash zone...
            m_window.draw(m_bgSprite);
            m_curGameState->render();//draw the grid...
            //m_window.draw(fpsText);
            m_window.display();

            //numFrames++;
        }

        numFrames++;
        sf::sleep(sf::milliseconds(1));

        //hack, don't understand the problem in linux enough
        //how to constrain min window size x11?
        //don't need to do this with windows though
        /*
        //no good fullscreen... 
        sdl?
#ifndef _WIN32
        {
            uint width = m_window.getSize().x;
            uint height = m_window.getSize().y;
            if(width < 16u * BOARD_WIDTH) width = 16u * BOARD_WIDTH;
            if(height < 16u * BOARD_HEIGHT) height = 16u * BOARD_HEIGHT;
            m_window.setSize({width,height});
        }
#endif
*/
    }
}

GameState::GameState(StateMgr &m, Context &context)
     : grid(*context.grid), window(*context.window), font(*context.font), 
     m_borderColor(context.borderColor), m_borderTexID(context.borderTexID),
     m_viewRect(context.viewRect), mgr(m)
{
    float cellSz = grid.getCellSize().x;
    
    m_bottomText.setFont(*context.font);
    m_bottomText.setCharacterSize(cellSz * 0.75f);
    m_bottomText.setFillColor(sf::Color(128,128,128,128));
}

void GameState::requestStateChange(STATE state, int arg)
{
    mgr.newStateWasRequested = true;
    mgr.nextState = state;
    mgr.action = REPLACE;
    mgr.stateArg = arg;
}

void GameState::requestStatePush(STATE state, int arg)
{
    mgr.newStateWasRequested = true;
    mgr.nextState = state;
    mgr.action = PUSH;
    mgr.stateArg = arg;
}

void GameState::requestStatePop()
{
    mgr.newStateWasRequested = true;
    mgr.action = POP;
}

GameState *GameState::getPushedState()
{
    return mgr.pOldState;
}

//default
void GameState::render()
{
    //window.clear(sf::Color::Black);
    grid.render(window);
}

void GameState::onPaint()
{
    //the view...
    for(unsigned int i = 0; i < m_viewRect.width; ++i)
        for(unsigned int j = 0; j < m_viewRect.height; ++j)
        {
            Coord C_grid = m_viewRect.toGlobal({i,j});//transform({i,j});
            grid.setCellColor(C_grid, sf::Color::Black);
            grid.setCellTexture(C_grid, getTextureUV(TexAtlasID::BACKGROUND_TEXTURE), getTextureSize());
        }
    
    paint();
}

void GameState::onEvent(const sf::Event &event)
{
    if(event.type == sf::Event::Resized)
    {
        float cellSz = grid.getCellSize().y;
        m_bottomText.setCharacterSize(cellSz * 0.75f);
        float textWidth = m_bottomText.getGlobalBounds().width;
//        m_bottomText.setOrigin(textWidth/2.f, 0);

        m_bottomText.setPosition((event.size.width - textWidth)/2.f, event.size.height - 2.f * cellSz - (m_viewRect.P.j * cellSz) - grid.getOffset().y);
    }
    handleEvent(event);
}

void GameState::setBottomText(const std::string &string)
{
    float cellSz = grid.getCellSize().y;
    m_bottomText.setString(string);
    float textWidth = m_bottomText.getGlobalBounds().width;
    float width = window.getSize().x;
    float height = window.getSize().y;
    m_bottomText.setPosition((width - textWidth)/2.f, height - 2.f * cellSz - (m_viewRect.P.j * cellSz) - grid.getOffset().y);
//    m_bottomText.setPosition((width - textWidth)/2.f, height - 2.f * cellSz - grid.getOffset().y);
}

void GameState::setViewRect(unsigned int px, unsigned int py, unsigned int w, unsigned int h)
{
    m_viewRect.set(px, py, w, h);
}

EditState::EditState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    window.setTitle("Editing");
    grid.clear(sf::Color::Red);
}