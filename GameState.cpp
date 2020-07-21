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
StateMgr::StateMgr() : m_grid(22,14,64.f,64.f)//m_grid(32,18,50.f)//m_grid(24,32,32)// 
{
    //set cellsz of grid after find out size of window...
    PGUI::setCellSize(32.f);
    //draw the grid with an offset?
    m_curGameState = nullptr;//???
    pOldState = nullptr;

//hope it won't throw...!
    m_context.window = &m_window;
    m_context.grid = &m_grid;
    m_context.board = &m_board;
    m_context.font = &m_font;
    m_context.texture = &m_texture;

    m_context.activeFolder = new Folder("Default");

    newStateWasRequested = true;
    nextState = WELCOME;
    action = REPLACE;
//is ok-ish

//    float factor = 1080.f / m_grid.getSize().y;
//    m_grid.create(22,14,m_grid.getCellSize()*factor);

    //set the window and grid sizes here... common to all states.
    float width = m_grid.getSize().x;
    float height = m_grid.getSize().y;

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
    m_window.create(sf::VideoMode(width, height), "Pentominos", sf::Style::Close);
    //m_window.setPosition({1920+(1920 - (int)width)/2 , (1080 - (int)height)/2});//hmmmm
    //m_window.setSize({(uint)width,(uint)height});//int -> unsigned int
    sf::View view;
    view.setCenter(width/2, height/2);
    view.setSize(width, height);
    m_window.setView(view);

    //load assets
    //TODO: TEST FOR LOADFAIL... what to do... abstract out?
    m_texture.loadFromFile("Assets/TexAtlas.png");//Elephant.png");//
    m_font.loadFromFile("Assets/FreeSans.ttf");

    m_stateMapping[WELCOME] = [this](int x)->GameState*{return new WelcomeState(*this, m_context);};
    m_stateMapping[MENU] = [this](int x)->GameState*{return new MenuState(*this, m_context);};
    m_stateMapping[PLAY] = [this](int x)->GameState*{return new PlayState(*this, m_context, x);};//where x = level
    //m_stateMapping[EDIT] = [this](int x)->GameState*{return new EditBoard(*this, m_context);};
    m_stateMapping[PLAYGROUND] = [this](int x)->GameState*{return new PlayGroundState(*this, m_context);};
    m_stateMapping[WIN] = [this] (int x)->GameState*{return new WinState(*this, m_context, (WINSTATETYPE)x);};//How to send additional DATA?
    m_stateMapping[HELP] = [this](int x)->GameState*{return new HelpState(*this, m_context);};
}

StateMgr::~StateMgr()
{
    delete m_curGameState;
    if(pOldState) delete pOldState;
    delete m_context.activeFolder;
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
            m_curGameState->handleEvent(event);
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) m_window.close();
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
            m_curGameState->render();//draw the grid...
            m_window.draw(fpsText);
            m_window.display();

            //numFrames++;
        }

        numFrames++;
        sf::sleep(sf::milliseconds(1));
    }
}

GameState::GameState(StateMgr &m, Context &context)
     : grid(*context.grid), window(*context.window), font(*context.font), 
     texture(*context.texture), mgr(m)
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

void GameState::setBottomText(const std::string &string)
{
    float cellSz = grid.getCellSize().y;
    m_bottomText.setString(string);
    float textWidth = m_bottomText.getGlobalBounds().width;
    float width = window.getSize().x;
    float height = window.getSize().y;
    m_bottomText.setPosition((width - textWidth)/2.f, height - 2.f * cellSz);
}

EditState::EditState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    window.setTitle("Editing");
    grid.clear(sf::Color::Red);
}