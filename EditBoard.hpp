#ifndef EDIT_BOARD_HPP
#define EDIT_BOARD_HPP

#include "GameState.hpp"
#include "Pentaminos.hpp"

//more a 'state'
//can keep the same 'grid' across the board.
//window not changing size

class EditBoard : public GameState
{
public:
    EditBoard(StateMgr &mgr, Context &context);
    //void run();
    void update() override;
    //void render();
    void handleEvent(const sf::Event &event) override;
private:
//    sf::RenderWindow window;
    sf::Vector2f mousePos;
    Coord mouseCoord;

    std::vector<uint32_t> m_data;//store in cell data...

    void draw(Coord C);
    void save();
};

#endif //EDIT_BOARD_HPP