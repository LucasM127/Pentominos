#include "MenuState.hpp"
#include "Level.hpp"

#include "TextBox.hpp"

#include <iostream>


#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
//right click to rename???

void MenuState::Icon::set(const Level &level, sf::Font &font)
{
    sf::Image image;
    image.create(24,24, sf::Color(0,0,0,0));
    uint x_off = (24 - level.width) / 2;
    uint y_off = (24 - level.height) / 2;

    for(unsigned int y = 0; y < level.height; ++y)
    {
        uint32_t data = reverseBits(level.data[y], level.width);
        for(unsigned int x = 0; x < level.width; ++x)
        {
            if(checkBit(data, x))
                image.setPixel(x + x_off, y + y_off, sf::Color(255,255,255,192));
        }
    }

    texture.loadFromImage(image);
//should maybe have a default texture in case it takes long to load a texture ?????
    sprite.setTexture(texture);
    sprite.setOrigin(12.f,12.f);
    sprite.scale(128.f/24.f,128.f/24.f);

    //only some way to set 'centered' multiline
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setString(level.name);
    text.setStyle(sf::Text::Underlined);
    float width = text.getLocalBounds().width;
    text.setOrigin(width/2,0);
}

bool MenuState::Icon::isHovered(const sf::Vector2f &mousePos)
{
    return sprite.getGlobalBounds().contains(mousePos);
}

MenuState::MenuState(StateMgr &mgr, Context &context)
    : GameState(mgr, context), pp_activeFolder(&context.activeFolder)//that is the problem with new!!!
{
    window.setTitle("Pentaminos");

    idSelected = -1;
    grid.clear(sf::Color::Black);

    loadFolder();
}

void MenuState::loadFolder()
{
    sf::Clock clock;
    m_icons.clear();
    m_icons.resize((*pp_activeFolder)->levels.size() + NUM_ICONS);//re-allocates and throws the textures out of scope if pushback or emplace back
    m_icons[ID_CREATELEVEL].set(Level::m_icons[0], font);
    m_icons[ID_CREATEFOLDER].set(Level::m_icons[2], font);
    m_icons[ID_SWITCHFOLDER].set(Level::m_icons[1], font);
    for(size_t i = 0; i < (*pp_activeFolder)->levels.size(); ++i)
    {
        m_icons[i+NUM_ICONS].set((*pp_activeFolder)->levels[i], font);
    }
    
    //position
    for(unsigned int i = 0; i < m_icons.size(); ++i)
    {
        float sz = grid.getCellSize().x;
        float x_pos = 2*sz + 3*sz * (i%7);
        float y_pos = 2*sz + 3*sz * (i/7);
        m_icons[i].sprite.setPosition(x_pos, y_pos);
        m_icons[i].text.setPosition(x_pos, y_pos + sz/2.f);
    }

    {
        m_folderText.setFont(font);
        m_folderText.setCharacterSize((unsigned int)grid.getCellSize().y);
        m_folderText.setFillColor(sf::Color::White);
        m_folderText.setString((*pp_activeFolder)->name);//"Default Levels");
        float width = m_folderText.getLocalBounds().width;
        m_folderText.setOrigin(width/2.f,0.f);
        m_folderText.setPosition(grid.getSize().x/2.f + grid.getOffset().x,0.f);
    }

    std::cout<<"Loading menu took "<<clock.restart().asMilliseconds()<<" ms\n";
}

void MenuState::handleEvent(const sf::Event &event)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if(event.type == sf::Event::MouseMoved)
    {
        idLastSelected = idSelected;
        idSelected = INVALID_ID;
        
        for(uint i = 0; i < m_icons.size(); i++)
        {
            auto &icon = m_icons[i];
            if(icon.isHovered(mousePos))
            {
                idSelected = i;
                break;
            }
        }
        if(idLastSelected != idSelected)
        {
            if(idSelected != INVALID_ID)
            {
                if(idSelected == ID_CREATELEVEL || idSelected == ID_CREATEFOLDER)
                    m_icons[idSelected].sprite.setColor(sf::Color::Red);
                else if(idSelected == ID_SWITCHFOLDER)
                    m_icons[idSelected].sprite.setColor(sf::Color::Yellow);
                else
                    m_icons[idSelected].sprite.setColor(sf::Color::Green);
                m_icons[idSelected].sprite.scale(1.25f,1.25f);
            }
            if(idLastSelected != INVALID_ID)// m_levelTexts[idLastSelected].setFillColor(sf::Color::White);
            {
                m_icons[idLastSelected].sprite.setColor(sf::Color::White);
                m_icons[idLastSelected].sprite.scale(0.8f,0.8f);
            }
        }
    }

    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(idSelected != INVALID_ID)
        {
            if(idSelected < NUM_ICONS)//add or remove???
            {
                if(idSelected == ID_SWITCHFOLDER)
                {
                    //list of names input....
                    std::string path = "Assets/Levels/";
                    std::vector<std::string> files;
                    for (const auto & entry : fs::directory_iterator(path))
                    {
                        std::string s = entry.path();
                        s.erase(s.begin(), s.begin() + path.size());
                        s.erase(s.end()-4, s.end());
                        files.push_back(s);
                    }
                    for(auto &s : files) std::cout<<s<<std::endl;
                    
                    //use filedirectory iterator...
                    PGUI::ListBox listbox(window, "List contents", font, files);//, "Folders...");
                    PGUI::MSG ret = listbox.run();
                    if(ret == PGUI::MSG::LIST_ITEM_CHOSE)
                    {
                        std::cout<<"Item chosen was: "<<files[listbox.getChosenId()]<<std::endl;
                        delete *pp_activeFolder;
                        *pp_activeFolder = new Folder(files[listbox.getChosenId()]);
                        loadFolder();
                    }
                }
                if(idSelected == ID_CREATELEVEL)
                    requestStateChange(PLAYGROUND);
                if(idSelected == ID_CREATEFOLDER)
                {
                    PGUI::TextBox textbox(window, L"Enter Folder Name", "", font);
                    PGUI::MSG ret = textbox.run();
                    if(ret == PGUI::MSG::TEXT_CHANGED)
                    {
                        delete *pp_activeFolder;
                        *pp_activeFolder = new Folder(textbox.getString());
                        //load it in the menu
                        loadFolder();
                    }
                }
            }
            else
            {
                if(event.mouseButton.button == sf::Mouse::Right)
                {
                    PGUI::TextBox textbox(window, L"Rename level", (*pp_activeFolder)->levels[idSelected-NUM_ICONS].name, font);//, [&]()->void{name = textbox.getString();});
                    PGUI::MSG ret = textbox.run();
                    //?
                    if(ret == PGUI::MSG::TEXT_CHANGED)
                    {
                        (*pp_activeFolder)->levels[idSelected-NUM_ICONS].name = textbox.getString();
                        (*pp_activeFolder)->save();
                        loadFolder();
                    }
                    if(ret == PGUI::MSG::DELETE)
                    {
                        (*pp_activeFolder)->levels.erase((*pp_activeFolder)->levels.begin() + (idSelected-NUM_ICONS));
                        (*pp_activeFolder)->save();
                        loadFolder();
                        idSelected = INVALID_ID;//no longer exists, duh
                    }
                    if(ret == PGUI::MSG::MOVE)
                    {//copy pasta
                        std::string path = "Assets/Levels/";
                        std::vector<std::string> files;
                        for (const auto & entry : fs::directory_iterator(path))
                        {
                            std::string s = entry.path();
                            s.erase(s.begin(), s.begin() + path.size());
                            s.erase(s.end()-4, s.end());
                            files.push_back(s);
                        }
                        for(auto &s : files) std::cout<<s<<std::endl;
                        
                        //use filedirectory iterator...
                        PGUI::ListBox listbox(window, "List contents", font, files);//, "Folders...");
                        PGUI::MSG ret = listbox.run();
                        if(ret == PGUI::MSG::LIST_ITEM_CHOSE && files[listbox.getChosenId()] != (*pp_activeFolder)->name)
                        {
                            //move it there...
                            Folder tempFolder(files[listbox.getChosenId()]);
                            tempFolder.levels.push_back((*pp_activeFolder)->levels[idSelected - NUM_ICONS]);
                            //remove it here...
                            (*pp_activeFolder)->levels.erase((*pp_activeFolder)->levels.begin() + idSelected - NUM_ICONS);
                            loadFolder();
                            //if we erase it and have the last file selected... our idSelected is invalidated...
                            idSelected = INVALID_ID;
                        }
                    }
                }
                else
                requestStateChange(PLAY, idSelected - NUM_ICONS);
            }
            
        }
    }

}

void MenuState::render()
{
    grid.render(window);
    window.draw(m_folderText);
    for(auto &icon : m_icons) window.draw(icon.sprite);
    if(idSelected != INVALID_ID)
        window.draw(m_icons[idSelected].text);
}