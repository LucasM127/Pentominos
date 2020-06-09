#include "GameState.hpp"
//#include "GameBoard.hpp"

//ie Paused state
enum WINSTATETYPE : int
{
    WON = 0,
    PAUSED
};

class WinState : public GameState
{
public:
    WinState(StateMgr &mgr, Context &context, WINSTATETYPE type);
    void handleEvent(const sf::Event &event) override;
    void render() override;//try using a shader? blur it a bit, brighten the colours too...
private:
    //sf::Shader m_shader;
    sf::Text m_winText;
    sf::Text m_continueText;
    sf::RenderTexture m_renderTexture;
    WINSTATETYPE m_type;
};
