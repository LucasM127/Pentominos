#include "GameState.hpp"

class MenuState : public GameState
{
public:
    MenuState(StateMgr &mgr, Context &context);
    void handleEvent(const sf::Event &event) override;
    void render() override;//try using a shader? blur it a bit, brighten the colours too...

    struct Icon
    {
        void set(const Level &level, sf::Font &font, float cellSz);
        sf::Text text;
        sf::Texture texture;
        sf::Sprite sprite;

        bool isHovered(const sf::Vector2f &mousePos);
    };
private:
    //bunch of texts
    sf::Text m_folderText;
    //hmm
    Folder **pp_activeFolder;
    std::vector<Icon> m_icons;//load the texture separately???
    uint idSelected, idLastSelected;
    enum : unsigned int
    {
        ID_CREATELEVEL = 0,
        ID_CREATEFOLDER,
        ID_SWITCHFOLDER,
        ID_HELP,
        NUM_ICONS
    };

    void loadFolder();
    void position();
    void updateId(uint id);
    void execute();

    sf::Vector2f mousePos;
};
