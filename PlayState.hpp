#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP

#include "GameState.hpp"
//include GameBoard?

class PlayState : public GameState
{
public:
    PlayState(StateMgr &mgr, Context &context, int lvl);
    ~PlayState();
    void handleEvent(const sf::Event &event) override;
    void update() override;
    void render() override;
    void paint() override;
    //void tick(){}//ie update but thinkging
    
private:
    sf::Text m_text;
    GameBoard &board;
    Controller m_controller;
    //Coord mouseCoord;
    DrawSettings m_drawSettings;
};

#endif //PLAYSTATE_HPP