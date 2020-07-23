#ifndef PLAYGROUNDSTATE_HPP
#define PLAYGROUNDSTATE_HPP

#include "GameState.hpp"

#include "TextBox.hpp"

//lots of common features to playstate but differences tooooooo
class PlayGroundState : public GameState
{
public:
    PlayGroundState(StateMgr &mgr, Context &context);
    ~PlayGroundState();
    void handleEvent(const sf::Event &event) override;
    //void update() override;
    void render() override;
    //void tick(){}//ie update but thinkging
    
private:
    GameBoard &board;
//    Coord mouseCoord;
    Controller m_controller;
    DrawSettings m_drawSettings;
    Folder *activeFolder;

    void save();

    PGUI::Button exit;
    bool isHovered;

    sf::Text m_text;//vs a gui button???
};

#endif //PLAYSTATE_HPP