#include "GameBoard.hpp"
#include "EditBoard.hpp"
#include <iostream>
#include <regex>
#include "Colors.hpp"
#include "GameState.hpp"

int main()
{
    StateMgr mgr;
    mgr.run();
    /*
    Grid grid(10,10,10);
    grid.clear(sf::Color::White);
    sf::RenderTexture texture;
    texture.create(100,100);
    grid.render(texture);
    texture.display();
    texture.getTexture().copyToImage().saveToFile("foo.png");*/
    
    /*
    sf::Texture *texture = new sf::Texture;
    //texture->loadFromFile("MarbleTex.png");
    texture->loadFromFile("Assets/TexAtlas.png");
    sf::Font font;
    font.loadFromFile("FreeSans.ttf");

    //ask for input...
    //while(true)
    
    {
        std::cout<<"Welcome to Pentaminos"<<std::endl;
        std::cout<<"Use mouse to move the shapes around and 'a' 'd' keys to rotate 'w' to flip"<<std::endl;
        std::cout<<"Levels"<<std::endl;
        std::cout<<"1:   3 x 20"<<std::endl;
        std::cout<<"2:   4 x 15"<<std::endl;
        std::cout<<"3:   5 x 12"<<std::endl;
        std::cout<<"4:   6 x 10"<<std::endl;
        std::cout<<"5:   8 x 8 with 2 x 2 hole"<<std::endl;
        std::cout<<"6:   8 x 8 with corners clipped"<<std::endl;
        std::cout<<"7:   Elephant"<<std::endl;
        std::cout<<"8:   Fish (solvable?)"<<std::endl;
        std::cout<<"9:   Elephant Head"<<std::endl;

        std::cout<<"Enter level number 0-8 or anything else to quit"<<std::endl;
        int num;
//        char input;
        std::cin >> num;//input;
        if(num >= 1 && num <= 10)//(input >= '1' && input <= '9')
        {
            num--;
            //num = input - '1';
            GameBoard game(texture, font, num);
            game.run();
        }
    }

    delete texture;*/

/*
    EditBoard editor;
    editor.run();
    */
/*** SAVE IMAGE TO TEXTURE KIND OF FUNCTION>>>
   //loading an elephant image from levell....
    sf::Image img;
    img.create(24,24);
    //use the same img function

    Level level(7);
    level.width;
    level.height;
    level.data.size();

    int y_offset = (22 - level.height)/4 + 1;
    int x_offset = (22 - level.width)/2;

    sf::Color color;

    for(int j = 0; j < 22; j++)
    {
        for(int i = 0; i < 22; i++)
        {
            srand((100 & 0xFF) << 24 |(i & 0xFFF) << 12 | (j & 0xFFF));
            int variation = rand()%16;
            color = sf::Color::Black;
            if(i >= x_offset && i < (x_offset + level.width) && j >= y_offset && j < (y_offset + level.height))//in range
            {
                if(checkBit(level.data[j - y_offset], i - x_offset))
                    color = sf::Color::White;
            }
            //color = variate(color, variation);
            img.setPixel(i,j,color);
        }
    }

    img.saveToFile("previewTexture.png");
    ***********/
    return 0;
}