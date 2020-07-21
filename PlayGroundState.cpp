#include "PlayGroundState.hpp"

#include "PlayState.hpp"

#include "GameBoard.hpp"
#include "Colors.hpp"

#include <list>
#include <iostream>

#define LOG(X)

//create 2 buttons bottom right for '(S)ave' 'E(x)it'
//how can that work?

PlayGroundState::PlayGroundState(StateMgr &mgr, Context &context)
    : GameState(mgr, context), board(*context.board), m_controller(board,grid), activeFolder(context.activeFolder)
{
    LOG("Created PlayGroundState");
    board.set(grid.getMapper(), Level::emptyLevel);//set to be with no winzone Level at all...
    context.window->setTitle(Level::emptyLevel.name);

    m_drawSettings.baseLightness = 0.6f;
    m_drawSettings.baseSaturation = 0.9f;

    //use the grid board draw function...
    CoordMapper &CM = board.CM;
    for(uint w = 0; w<CM.width; w++)
        for(uint h = 0; h<CM.height;h++)
        {
            m_drawSettings.draw({w,h}, grid, board);
        }

    //buttons?
    {
    unsigned int width = grid.getMapper().width;
    unsigned int height = grid.getMapper().height;
    exit.set(width - 2 , width - 1, height - 1, height - 1);
    exit.color = COLOR::brighten(sf::Color(255,0,0,128), 0.8f);//1.25
    isHovered = false;

    grid.setCellColor({width-1, 0}, sf::Color::White);
    grid.setCellTexture({width-1, 0}, getTextureUV(EMPTY), getTextureSize(), Orientation::DEFAULT, false);
    }
    {
    setBottomText("Press [S] to save or [Q] to quit");
    }
}

PlayGroundState::~PlayGroundState(){}

//COMMON over PlayState and PlayGroundPlay State (tho PlayGround has different event for saving!)
void PlayGroundState::handleEvent(const sf::Event &event)
{
    //manages it.  but is NOT a state
    //board.handleEvent(event); ??? more a grid Manager
    //or just have it as GUI->handleEvent() then Board->handleEvent()
    //Board->render()?  but why it is not the boards job
    //State-> board update  This is shared too...
    //feel like should inherit board manipulation ... public BoardManipulator
    //then draw is different for each, but not too different
    //handleevent-> GUI->handle event, then BaseManipulator->handleEvent, then local->handle event
    switch (event.type)
    {
    case sf::Event::KeyPressed:
        switch (event.key.code)
        {
        case sf::Keyboard::Q:
        case sf::Keyboard::Space:
            requestStateChange(MENU);
            break;
        case sf::Keyboard::S://printout the thing!
            save();
            requestStateChange(MENU);
            break;
        default:
            break;
        }
        break;
    case sf::Event::MouseMoved:
        {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        Coord mouseCoord = grid.getCoordinate(mousePos);
        isHovered = exit.isHovered(mouseCoord);
        }
        break;
    case sf::Event::MouseButtonPressed:
    {
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            if(isHovered) requestStateChange(MENU);
        }
    }
    default:
        break;
    }

    m_controller.handleEvent(event);
    m_controller.draw(m_drawSettings);
}

void PlayGroundState::update()
{
    if(isHovered)
        exit.brighten(grid);
    else
        exit.setColor(grid);
}

void PlayGroundState::render()
{
    grid.render(window);
    window.draw(m_bottomText);
}

void PlayGroundState::save()
{
    std::string name;
    PGUI::TextBox textbox(window, L"Save level as...", "", font);//, [&](){name = textbox.getString();});
    PGUI::MSG ret = textbox.run();
    if(ret != PGUI::MSG::TEXT_CHANGED) return;
    name = textbox.getString();

    const std::vector<uint8_t> &data = board.m_data;
    CoordMapper &CM = board.CM;

    //for each line...
    //print out the numbers!
    std::vector<uint32_t> map;
    unsigned int min_i = CM.width;
    unsigned int max_i = 0;
    for(unsigned int j = 0; j<CM.height; j++)
    {
        uint32_t k = 0;//the key lol
        for(unsigned int i = 0; i < CM.width; i++)
        {
            k = k << 1;//0 = 0 for first iteration no worries
            if((data[CM.mapID({i,j})]&0x0F) < 12)//or use a function??? inline probs
            {
                if(i < min_i) min_i = i;
                if(i > max_i) max_i = i;
                k |= 1;
            }
        }
        map.push_back(k);
    }
    while (map.front() == 0)
        map.erase(map.begin());
//        map.pop_front();
    while (map.back() == 0)
        map.pop_back();
    for(auto &n : map)
        n = n >> (CM.width - max_i - 1);

    for(auto n : map)
        std::cout<<n<<std::endl;
    std::cout<<"min max i : "<<min_i<<" "<<max_i<<std::endl;
    uint width = max_i - min_i + 1;//indice inclusive
    uint height = map.size();
    std::cout<<"width "<<width<<" height "<<height<<std::endl;
    //std::cout<<"min max j : "<<min_j<<" "<<max_j<<std::endl;
    //yay
    //Now I can save this to a level?  Hmmmmmm
    activeFolder->levels.emplace_back(name,width,height,map);
    //save to file
    activeFolder->save();
//    saveLevels(levels, levelFileName);

    //save to its name!
}

/***

//Calculates WinZoneMap Save File numbers for the save file, with width/height!
//works
void PlayGroundState::foo()
{
    const std::vector<uint8_t> &data = board.getData();
    CoordMapper &CM = board.CM;

    //for each line...
    //print out the numbers!
    std::vector<uint32_t> map;
    unsigned int min_i = CM.width;
    unsigned int max_i = 0;
    for(unsigned int j = 0; j<CM.height; j++)
    {
        uint32_t k = 0;//the key lol
        for(unsigned int i = 0; i < CM.width; i++)
        {
            k = k << 1;//0 = 0 for first iteration no worries
            if((data[CM.mapID({i,j})]&0x0F) < 12)//or use a function??? inline probs
            {
                if(i < min_i) min_i = i;
                if(i > max_i) max_i = i;
                k |= 1;
            }
        }
        map.push_back(k);
    }
    while (map.front() == 0)
        map.erase(map.begin());
//        map.pop_front();
    while (map.back() == 0)
        map.pop_back();
    for(auto &n : map)
        n = n >> (CM.width - max_i - 1);

    for(auto n : map)
        std::cout<<n<<std::endl;
    std::cout<<"min max i : "<<min_i<<" "<<max_i<<std::endl;
    uint width = max_i - min_i + 1;//indice inclusive
    uint height = map.size();
    std::cout<<"width "<<width<<" height "<<height<<std::endl;
    //std::cout<<"min max j : "<<min_j<<" "<<max_j<<std::endl;
    //yay
    //Now I can save this to a level?  Hmmmmmm
    Level lvl("Custom",width,height,map);
    std::ofstream file("Assets/Levels", std::ios::app);
    if(!file) throw;
        saveLevel(file, lvl);
    file.close();

}***/