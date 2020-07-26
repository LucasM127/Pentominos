#ifndef HELPSTATE_HPP
#define HELPSTATE_HPP

#include "GameState.hpp"

class HelpState : public GameState
{
public:
    HelpState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
    void render() override;
private:
    //bunch of texts
    std::vector<sf::Text> m_texts;
    sf::Text m_instructionText;
    sf::Sprite sprite;
    sf::Texture texture;

    void position();
    void paint() override;
};

#endif //HELPSTATE_HPP