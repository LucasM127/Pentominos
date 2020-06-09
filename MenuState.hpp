#include "GameState.hpp"

class MenuState : public GameState
{
public:
    MenuState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
    void render() override;//try using a shader? blur it a bit, brighten the colours too...
private:
    //bunch of texts
    std::vector<sf::Text> m_levelTexts;
    sf::Font &font;
    uint idSelected, idLastSelected;
};
