#include "PlayState.hpp"

#include "GameBoard.hpp"
#include "Colors.hpp"

#include <iostream>

#define LOG(X)

// i hand it a reference or some link to the level...
// in the Gamestate Class?

//get hue from lvl?
//or set the palette from numPieces = 12

//Bug when reset the piece... not sure if want to keep anyways...
//When touching the winzone piece, if dbl-click, perhaps just clear it off the winzone (or if selected clear off winzone too)

//int lvl... thinking about this....
PlayState::PlayState(StateMgr &mgr, Context &context, int lvl)
    : GameState(mgr, context), board(*context.board), m_controller(board, grid, m_viewRect, context.borderColor, context.borderTexID)/*controller(*context.controller)*/
{
    //tell the controller or drawer or something the board view offset to use...

    LOG("Created PlayState");
    //Level L = (*context.levels)[lvl];

    board.set(grid.getMapper(), context.activeFolder->levels[lvl]);

    window.setTitle(context.activeFolder->levels[lvl].name);//Level::m_preLoadedLevels[lvl].name);

    setBottomText("Press [Q] to quit");

    //use the grid board draw function...
    /*
    CoordMapper &CM = board.CM;
    for(uint w = 0; w<CM.width; w++)
        for(uint h = 0; h<CM.height;h++)
        {
            m_drawSettings.draw({w,h}, grid, board, m_viewRect);
        }
    */
   onPaint();
}

PlayState::~PlayState(){}

//COMMON over PlayState and PlayGroundPlay State (tho PlayGround has different event for saving!)
void PlayState::handleEvent(const sf::Event &event)
{
    //requestStateChange(WIN);
    //manages it.  but is NOT a state
    //board.handleEvent(event); ??? more a grid Manager
    switch (event.type)
    {
    case sf::Event::KeyPressed:
        switch (event.key.code)
        {
        case sf::Keyboard::Q:
        case sf::Keyboard::Space:
            requestStateChange(MENU);
            break;
            /*
        case sf::Keyboard::A:
            board.rotatePieceRight();
            break;
        case sf::Keyboard::D:
            board.rotatePieceLeft();
            break;
        case sf::Keyboard::W:
            board.flipPiece();
            break;*/
        default:
            break;
        }
        break;
        /*
    case sf::Event::MouseMoved:
        {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouseCoord = grid.getCoordinate(mousePos);
        //board.movePiece(mouseCoord);//setActiveCoord?
        board.setActive(mouseCoord);
        }
        break;
    case sf::Event::MouseButtonPressed:
    {
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            board.pickUpOrPlacePiece();
        }
        else if(event.mouseButton.button == sf::Mouse::Right)
        {
            board.resetPiece();
        }
    }*/
    default:
        break;
    }
    //mouse position in event structure, so no need window here!
    m_controller.handleEvent(event);

    m_controller.draw(m_drawSettings);//that might be better
}

void PlayState::update()
{
    //board.update();

    //updateGrid();

    if(board.won())
        //m_text.setString("YAY!");
        requestStateChange(WIN);
    //else
    //    m_text.setString("Keep Trying...");
}

void PlayState::render()
{
    grid.render(window);
    window.draw(m_bottomText);
}

void PlayState::paint()
{
    //redraw the winzone
    //redraw all the pieces
    //and the hovered piece.
    //use the active controller to do it.
    m_controller.drawAll(m_drawSettings);
}