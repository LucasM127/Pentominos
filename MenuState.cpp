#include "MenuState.hpp"
#include "Level.hpp"

#include "TextBox.hpp"

#include <iostream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
//right click to rename???

void MenuState::Icon::set(const Level &level, sf::Font &font, float cellSz)
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

    texture.loadFromImage(image);//super fast
    //sf::sleep(sf::milliseconds(200));
//should maybe have a default texture in case it takes long to load a texture ?????
    sprite.setTexture(texture);
    sprite.setOrigin(12.f,12.f);
    
    sprite.scale(cellSz * 2.f/24.f,cellSz * 2.f/24.f);//2 * cellsize...

    //only some way to set 'centered' multiline
    text.setFont(font);
    text.setCharacterSize(cellSz/3.f);
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

    idLastSelected = -1;
    idSelected = 0;//-1;
    //grid.clear(sf::Color::Black);
    onPaint();

    loadFolder();

    setBottomText("Right click to edit level");

    updateId(idSelected);
}

void MenuState::loadFolder()
{
    float cellSz = grid.getCellSize().x;
    sf::Clock clock;
    m_icons.clear();
    m_icons.resize((*pp_activeFolder)->levels.size() + NUM_ICONS);//re-allocates and throws the textures out of scope if pushback or emplace back
    m_icons[ID_CREATELEVEL].set(Level::m_icons[0], font, cellSz);
    m_icons[ID_CREATEFOLDER].set(Level::m_icons[2], font, cellSz);
    m_icons[ID_SWITCHFOLDER].set(Level::m_icons[1], font, cellSz);
    m_icons[ID_HELP].set(Level::m_icons[3], font, cellSz);
    for(size_t i = 0; i < (*pp_activeFolder)->levels.size(); ++i)
    {
        m_icons[i+NUM_ICONS].set((*pp_activeFolder)->levels[i], font, cellSz);
    }

    {
        m_folderText.setFont(font);
        m_folderText.setFillColor(sf::Color::White);
        m_folderText.setString((*pp_activeFolder)->name);//"Default Levels");
        float width = m_folderText.getLocalBounds().width;
        m_folderText.setOrigin(width/2.f,0.f);
    }

    position();

    //std::cout<<"Loading menu took "<<clock.restart().asMilliseconds()<<" ms\n";
}

void MenuState::position()
{
    //in view area...
    //position & scale...
    //icon function? if it becomes a pain maybe
    float sz = grid.getCellSize().x;
    for(unsigned int i = 0; i < m_icons.size(); ++i)
    {
        float x_pos = 2*sz + 3*sz * (i%7) + grid.getOffset().x + m_viewRect.P.i * sz;;//add the padding of our view
        float y_pos = 2*sz + 3*sz * (i/7) + grid.getOffset().y + m_viewRect.P.j * sz;;//these are 'integer' values assuming cellsz is integer
        m_icons[i].sprite.setPosition(x_pos, y_pos);
        m_icons[i].sprite.setScale(2.f*sz / 24.f, 2.f*sz / 24.f);
        m_icons[i].text.setPosition(x_pos, y_pos + sz/2.f);//set text sz (fn of cellsize)
        m_icons[i].text.setCharacterSize(sz/3.f);
        float width = m_icons[i].text.getLocalBounds().width;
        m_icons[i].text.setOrigin(width/2,0);
    }

    {
        m_folderText.setCharacterSize((unsigned int)grid.getCellSize().y);
        m_folderText.setPosition(grid.getSize().x/2.f + grid.getOffset().x, grid.getOffset().y  + m_viewRect.P.j * sz);
        float width = m_folderText.getLocalBounds().width;
        m_folderText.setOrigin(width/2.f,0.f);
    }
}

void MenuState::handleEvent(const sf::Event &event)
{
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if(event.type == sf::Event::Resized)
        position();
    if(event.type == sf::Event::MouseMoved)
    {
        //iff???
        
        //idSelected = INVALID_ID;
        for(uint i = 0; i < m_icons.size(); i++)
        {
            auto &icon = m_icons[i];
            if(icon.isHovered(mousePos))
            {
                idSelected = i;
                break;
            }
        }
        updateId(idSelected);
        /*
        idLastSelected = idSelected;
        if(idLastSelected != idSelected)
        {
            if(idSelected != INVALID_ID)
            {
                if(idSelected == ID_CREATELEVEL && m_icons.size()>=28)
                    m_icons[idSelected].sprite.setColor(sf::Color(128,128,128));
                else if(idSelected == ID_CREATELEVEL || idSelected == ID_CREATEFOLDER || idSelected == ID_SWITCHFOLDER)
                    m_icons[idSelected].sprite.setColor(sf::Color::Yellow);
//                else if(idSelected == ID_SWITCHFOLDER)
//                    m_icons[idSelected].sprite.setColor(sf::Color(255,128,0));
                else if(idSelected == ID_HELP)
                    m_icons[idSelected].sprite.setColor(sf::Color::Red);
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
        */
    }

    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(!m_icons[idSelected].isHovered(mousePos)) return;
        if(event.mouseButton.button == sf::Mouse::Left)
            execute();
        if(event.mouseButton.button == sf::Mouse::Right && idSelected >= NUM_ICONS)
        {
            PGUI::TextBoxDB textbox(*this, window, L"Rename level", (*pp_activeFolder)->levels[idSelected-NUM_ICONS].name, font);
            //PGUI::TextBox textbox(window, L"Rename level", (*pp_activeFolder)->levels[idSelected-NUM_ICONS].name, font);//, [&]()->void{name = textbox.getString();});
            PGUI::MSG ret = textbox.run();
            //?
            if(ret == PGUI::MSG::TEXT_CHANGED)
            {//can optimize this
                (*pp_activeFolder)->levels[idSelected-NUM_ICONS].name = textbox.getString();
                (*pp_activeFolder)->save();
                loadFolder();
            }
            if(ret == PGUI::MSG::DELETE)
            {
                (*pp_activeFolder)->levels.erase((*pp_activeFolder)->levels.begin() + (idSelected-NUM_ICONS));
                (*pp_activeFolder)->save();
                loadFolder();
                idSelected--;
                idLastSelected = INVALID_ID;
                updateId(idSelected);
//                        idSelected = INVALID_ID;//no longer exists, duh
            }
            if(ret == PGUI::MSG::MOVE)
            {//copy pasta
                std::string path = "Assets/Levels/";
                std::vector<std::string> files;
                for (const auto & entry : fs::directory_iterator(path))
                {
#ifdef _WIN32
                    std::wstring ws = entry.path();
                    std::string s;
                    for (auto c : ws) s.push_back(c);
#else
                    std::string s = entry.path();
#endif
                    s.erase(s.begin(), s.begin() + path.size());
                    s.erase(s.end()-4, s.end());
                    files.push_back(s);
                }
                //for(auto &s : files) std::cout<<s<<std::endl;
                
                //use filedirectory iterator...
                PGUI::ListBoxDB listbox(*this, window, L"List contents", font, files);//, "Folders...");
                PGUI::MSG ret = listbox.run();
                if(ret == PGUI::MSG::LIST_ITEM_CHOSE && files[listbox.getChosenId()] != (*pp_activeFolder)->name)
                {
                    //move it there...
                    Folder tempFolder(files[listbox.getChosenId()]);
                    if (tempFolder.levels.size() >= 24)
                    {
                        PGUI::DialogBox DB(12,2,*this, window, font, L"File is full!");
                        DB.run();
                        return;
                    }
                    tempFolder.levels.push_back((*pp_activeFolder)->levels[idSelected - NUM_ICONS]);
                    //remove it here...
                    (*pp_activeFolder)->levels.erase((*pp_activeFolder)->levels.begin() + idSelected - NUM_ICONS);
                    loadFolder();
                    //if we erase it and have the last file selected... our idSelected is invalidated...
                    idSelected--;
                    idLastSelected = INVALID_ID;
                    updateId(idSelected);
//                            idSelected = INVALID_ID;
                }
            }
        }
    }
    if(event.type == sf::Event::KeyPressed)
    {
        if(event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
        {
            idSelected++;
            idSelected %= m_icons.size();
            updateId(idSelected);
        }
        if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)
        {
            idSelected = (idSelected ? idSelected-1 : m_icons.size()-1);
            updateId(idSelected);
//            idSelected %= m_icons.size();
        }
        if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
        {//width of 7 hardcoded in at the moment
            idSelected = (idSelected > 6) ? (idSelected - 7) : (idSelected);
            updateId(idSelected);
        }
        if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
        {//width of 7 hardcoded in at the moment
            idSelected = (m_icons.size() - idSelected) > 7 ? (idSelected + 7) : (idSelected);
            updateId(idSelected);
        }
        if(event.key.code == sf::Keyboard::Enter)
        {
            execute();
        }
            
    }

}

void MenuState::updateId(uint id)
{
    if(idSelected == idLastSelected) return;
    //if(id == idSelected) return;
    
    if(idLastSelected != idSelected)
    {
        if(idSelected != INVALID_ID)
        {
            if(idSelected == ID_CREATELEVEL && m_icons.size()>=28)
                m_icons[idSelected].sprite.setColor(sf::Color(128,128,128));
            else if(idSelected == ID_CREATELEVEL || idSelected == ID_CREATEFOLDER || idSelected == ID_SWITCHFOLDER)
                m_icons[idSelected].sprite.setColor(sf::Color::Yellow);
//                else if(idSelected == ID_SWITCHFOLDER)
//                    m_icons[idSelected].sprite.setColor(sf::Color(255,128,0));
            else if(idSelected == ID_HELP)
                m_icons[idSelected].sprite.setColor(sf::Color::Red);
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

    idLastSelected = idSelected;
}

void MenuState::execute()
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
#ifdef _WIN32
                std::wstring ws = entry.path();
                std::string s;
                for (auto c : ws) s.push_back(c);
#else
                std::string s = entry.path();
#endif
                s.erase(s.begin(), s.begin() + path.size());
                s.erase(s.end()-4, s.end());
                files.push_back(s);
            }
            //for(auto &s : files) std::cout<<s<<std::endl;
            
            //use filedirectory iterator...
            PGUI::ListBoxDB listbox(*this, window, L"List contents", font, files);
            //PGUI::ListBox listbox(window, "List contents", font, files);//, "Folders...");
            PGUI::MSG ret = listbox.run();
            if(ret == PGUI::MSG::LIST_ITEM_CHOSE)
            {
                //std::cout<<"Item chosen was: "<<files[listbox.getChosenId()]<<std::endl;
                delete *pp_activeFolder;
                *pp_activeFolder = new Folder(files[listbox.getChosenId()]);
                loadFolder();
            }
        }
        if(idSelected == ID_CREATELEVEL)
        {
            //PGUI::TextBoxDB DB(*this, window, L"Tst Title", "Foo", font);
            //PGUI::MSG msg = DB.run();
            //std::cout<<PGUI::MSGStrings[(uint)msg]<<std::endl;
//                    PGUI::DialogBox DB(12, 3, *this, window, font, L"Test Title");

            if(m_icons.size()<28)
            requestStateChange(PLAYGROUND);
        }
        if(idSelected == ID_CREATEFOLDER)
        {
            PGUI::TextBoxDB textbox(*this, window, L"Enter Folder Name", "", font);
            //PGUI::TextBox textbox(window, L"Enter Folder Name", "", font);
            PGUI::MSG ret = textbox.run();
            if(ret == PGUI::MSG::TEXT_CHANGED)
            {
                delete *pp_activeFolder;
                *pp_activeFolder = new Folder(textbox.getString());
                //load it in the menu
                loadFolder();
            }
        }
        if(idSelected == ID_HELP)
        {
            requestStateChange(HELP);
        }
    }
    else
    {
        requestStateChange(PLAY, idSelected - NUM_ICONS);
    }
}

void MenuState::render()
{
    grid.render(window);
    window.draw(m_folderText);
    for(auto &icon : m_icons) window.draw(icon.sprite);
    if(idSelected != INVALID_ID)
        window.draw(m_icons[idSelected].text);
    window.draw(m_bottomText);
}