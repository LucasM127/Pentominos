#ifndef EDIT_BOARD_HPP
#define EDIT_BOARD_HPP

#include "GameState.hpp"

//not an editor, but an intro...
class WelcomeState : public GameState
{
public:
    WelcomeState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
    void paint() override;
private:
//    sf::Vector2f mousePos;
//    Coord mouseCoord;

    std::vector<uint32_t> m_data;//store in cell data...

    void draw(Coord C);
//    void save();
};

#endif //EDIT_BOARD_HPP