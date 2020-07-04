#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Grid.hpp"
#include "GameBoard.hpp"
#include <unordered_map>//map enum to new type...
#include <functional>

class PWindow : public sf::RenderWindow
{
private:
    void onCreate() override;
};

//kinda like my globals
struct Context
{
    sf::RenderWindow *window;
    Grid *grid;
    GameBoard *board;
    Controller *controller;
    sf::Font *font;
    sf::Texture *texture;
    std::vector<Level> *levels;
    //For coloring the gameboard pieces
    std::vector<float> hues;
    uint alpha;
};

enum STATE
{
    WELCOME,
    MENU,
    PLAY,
    EDIT,
    WIN,
    PLAYGROUND
};

enum STATE_ACTION
{
    PUSH,
    POP,
    REPLACE
};

class GameState;

//ie the APP
class StateMgr;

//can only be created by the StateMgr
class GameState
{
    friend class StateMgr;
public:
    virtual ~GameState(){}
    virtual void init(){}
    virtual void exit(){}
    virtual void handleEvent(const sf::Event &event){}
    virtual void update(){}
    virtual void render();
    virtual void tick(){}//ie update but thinkging
protected:
    Grid &grid;//common grid across the states... just displays different things...
    sf::RenderWindow &window;
    sf::Font &font;//or resource allocator thingy
    sf::Texture &texture;

    GameState(StateMgr &m, Context &context);
    //void requestStateChange(STATE newState, GameState *pOldState = nullptr);//pass a pointer???
    void requestStateChange(STATE state, int arg = 0);//, GameState *pOldState);//friend fucntion?
    void requestStatePush(STATE state, int arg = 0);
    void requestStatePop();
    GameState *getPushedState();
private:
    StateMgr &mgr;
};

class StateMgr
{
public:
    StateMgr();
    ~StateMgr();
    void run();
    //hmmm
    //well want it to be only callable by a GameState
    friend void GameState::requestStateChange(STATE state, int arg);//clears the old if isn't nullptr...
    friend void GameState::requestStatePush(STATE state, int arg);
    friend void GameState::requestStatePop();
    friend GameState *GameState::getPushedState();
private:

  // the stack of states
  GameState *m_curGameState;
//  PWindow m_window;
  sf::RenderWindow m_window;
  Grid m_grid;
  GameBoard m_board;
  sf::Font m_font;
  sf::Texture m_texture;
  std::vector<Level> m_levels;

  bool newStateWasRequested;
  STATE_ACTION action;//BETTER NAME PLEASE
  STATE nextState;
  GameState *pOldState;
  int stateArg;//int
  Context m_context;
  
  std::unordered_map<STATE, std::function<GameState*(int)> > m_stateMapping;
};

class WelcomeState : public GameState
{
public:
    WelcomeState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
};

class EditState : public GameState
{
public:
    EditState(StateMgr &mgr, Context &context);
    //void handleEvent(const sf::Event &event) override;
};

#endif //GAMESTATE_HPP