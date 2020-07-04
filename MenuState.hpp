#include "GameState.hpp"

class MenuState : public GameState
{
public:
    MenuState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
    void render() override;//try using a shader? blur it a bit, brighten the colours too...

    struct Icon
    {
        void set(const Level &level, sf::Font &font);
        sf::Text text;
        sf::Texture texture;
        sf::Sprite sprite;

        bool isHovered(const sf::Vector2f &mousePos);
    };
private:
    //bunch of texts
    sf::Text m_folderText;
    std::vector<Level> &levels;
    std::vector<Icon> m_icons;//yay! -> more really need a name, and winzone map
    uint idSelected, idLastSelected;
    const uint ID_CREATE, ID_SWITCHFOLDER;
};
