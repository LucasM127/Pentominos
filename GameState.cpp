#include "GameState.hpp"
#include "EditBoard.hpp"
#include "GameBoard.hpp"
#include "WinState.hpp"
#include "MenuState.hpp"
#include <iostream>
#include <cassert>
//16x9
//goes weird if width ever exceeds 32 lol -> cause of winShape as uint32_t
StateMgr::StateMgr() : m_grid(22,14,60.f)//m_grid(32,18,50.f)//m_grid(22,14,60.f)
{
    m_curGameState = nullptr;//???
    pOldState = nullptr;
    m_context.window = &m_window;
    m_context.grid = &m_grid;
    m_context.font = &m_font;
    m_context.texture = &m_texture;

    newStateWasRequested = true;
    nextState = EDIT;
    action = REPLACE;

    //set the window and grid sizes here... common to all states.
    int width = m_grid.getSize().x;
    int height = m_grid.getSize().y;
    m_window.create(sf::VideoMode(width, height), "Pentominos", sf::Style::Close);

    //load assets
    //TODO: TEST FOR LOADFAIL... what to do... abstract out?
    m_texture.loadFromFile("Assets/TexAtlas.png");
    m_font.loadFromFile("Assets/FreeSans.ttf");

    m_stateMapping[WELCOME] = [this](int x)->GameState*{return new WelcomeState(*this, m_context);};
    m_stateMapping[MENU] = [this](int x)->GameState*{return new MenuState(*this, m_context);};
    m_stateMapping[PLAY] = [this](int x)->GameState*{return new GameBoard(*this, m_context, x);};//where x = level
    m_stateMapping[EDIT] = [this](int x)->GameState*{return new EditBoard(*this, m_context);};
    m_stateMapping[WIN] = [this] (int x)->GameState*{return new WinState(*this, m_context, (WINSTATETYPE)x);};//How to send additional DATA?

}

StateMgr::~StateMgr()
{
    delete m_curGameState;
    if(pOldState) delete pOldState;
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

            numFrames++;
        }

        //numFrames++;
        sf::sleep(sf::milliseconds(1));
    }
}

GameState::GameState(StateMgr &m, Context &context)
     : grid(*context.grid), window(*context.window), font(*context.font), 
     texture(*context.texture), mgr(m)
{}

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
    //window.display();
}

PlayState::PlayState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    grid.clear(sf::Color::Blue);
}

WelcomeState::WelcomeState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    grid.clear(sf::Color::Green);
}

//make i t a green board fro welcome screen !
void WelcomeState::handleEvent(const sf::Event &event)
{
    if(event.type == sf::Event::KeyPressed)
        requestStateChange(MENU);
//        requestStateChange(PLAY, 9);
}

EditState::EditState(StateMgr &mgr, Context &context)
    : GameState(mgr, context)
{
    window.setTitle("Editing");
    grid.clear(sf::Color::Red);
}