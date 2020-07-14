SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
INC_SFML=-I $(SFML)/include
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib -lstdc++fs
OBJS=main.o WelcomeState.o WinState.o MenuState.o PlayState.o PlayGroundState.o GameState.o TextBox.o GameBoard.o Level.o Pentaminos.o Grid.o Colors.o LehmerRandom.o

test: $(OBJS)
	g++ -g -Wall $(OBJS) -o test $(LINK)

main.o: main.cpp GameState.o
	g++ -g -Wall main.cpp -c $(INC_SFML)

#the gamestates
WelcomeState.o: WelcomeState.cpp WelcomeState.hpp GameState.o
	g++ -g -Wall WelcomeState.cpp -c $(INC_SFML)

WinState.o: WinState.cpp WinState.hpp GameState.o
	g++ -g -Wall WinState.cpp -c $(INC_SFML)

MenuState.o: MenuState.cpp MenuState.hpp TextBox.o GameState.o
	g++ -g -Wall MenuState.cpp -c $(INC_SFML)

PlayState.o: PlayState.cpp PlayState.hpp GameState.o
	g++ -g -Wall PlayState.cpp -c $(INC_SFML)

PlayGroundState.o: PlayGroundState.cpp PlayGroundState.hpp TextBox.o GameState.o
	g++ -g -Wall PlayGroundState.cpp -c $(INC_SFML)

GameState.o: GameState.cpp GameState.hpp GameBoard.o
	g++ -g -Wall GameState.cpp -c $(INC_SFML)

#Dialog box gui makeshift elements, independent of states
TextBox.o: TextBox.cpp TextBox.hpp Colors.o Grid.o
	g++ -g -Wall TextBox.cpp -c $(INC_SFML)

#Combines the pentamino pieces, level, and grid together, manage and draw routine
GameBoard.o: GameBoard.cpp GameBoard.hpp Pentaminos.o Grid.o Level.o
	g++ -g -Wall GameBoard.cpp -c $(INC_SFML)

#Independent, file management
Level.o: Level.hpp Level.cpp
	g++ -g -Wall Level.cpp -c

#dependant on grid.hpp for Coord struct definition
Pentaminos.o: Pentaminos.cpp Pentaminos.hpp BitFuncs.hpp Grid.hpp
	g++ -g -Wall Pentaminos.cpp -c $(INC_SFML)

#The grid
Grid.o: Grid.cpp Grid.hpp Colors.o LehmerRandom.o
	g++ -g -Wall Grid.cpp -c $(INC_SFML)

Colors.o: Colors.cpp Colors.hpp
	g++ -g -Wall Colors.cpp -c $(INC_SFML)

LehmerRandom.o: LehmerRandom.cpp LehmerRandom.hpp
	g++ -g -Wall LehmerRandom.cpp -c
################

clean:
	rm *.o test