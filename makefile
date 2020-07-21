CC=g++ -g -Wall -pedantic
SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
INC_SFML=-I $(SFML)/include
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib -lstdc++fs
OBJS=main.o WelcomeState.o WinState.o MenuState.o PlayState.o PlayGroundState.o HelpState.o GameState.o TextBox.o GameBoard.o Level.o Pentaminos.o Grid.o Colors.o LehmerRandom.o

test: $(OBJS)
	$(CC) $(OBJS) -o test $(LINK)

main.o: main.cpp GameState.o
	$(CC) main.cpp -c $(INC_SFML)

#the gamestates
WelcomeState.o: WelcomeState.cpp WelcomeState.hpp GameState.o
	$(CC) WelcomeState.cpp -c $(INC_SFML)

WinState.o: WinState.cpp WinState.hpp GameState.o
	$(CC) WinState.cpp -c $(INC_SFML)

MenuState.o: MenuState.cpp MenuState.hpp TextBox.o GameState.o
	$(CC) MenuState.cpp -c $(INC_SFML)

PlayState.o: PlayState.cpp PlayState.hpp GameState.o
	$(CC) PlayState.cpp -c $(INC_SFML)

PlayGroundState.o: PlayGroundState.cpp PlayGroundState.hpp TextBox.o GameState.o
	$(CC) PlayGroundState.cpp -c $(INC_SFML)

HelpState.o: HelpState.cpp HelpState.hpp GameState.o
	$(CC) HelpState.cpp -c $(INC_SFML)

GameState.o: GameState.cpp GameState.hpp GameBoard.o
	$(CC) GameState.cpp -c $(INC_SFML)

#Dialog box gui makeshift elements, independent of states
TextBox.o: TextBox.cpp TextBox.hpp Colors.o Grid.o
	$(CC) TextBox.cpp -c $(INC_SFML)

#Combines the pentamino pieces, level, and grid together, manage and draw routine
GameBoard.o: GameBoard.cpp GameBoard.hpp Pentaminos.o Grid.o Level.o
	$(CC) GameBoard.cpp -c $(INC_SFML)

#Independent, file management
Level.o: Level.hpp Level.cpp
	$(CC) Level.cpp -c

#dependant on grid.hpp for Coord struct definition
Pentaminos.o: Pentaminos.cpp Pentaminos.hpp BitFuncs.hpp Grid.hpp
	$(CC) Pentaminos.cpp -c $(INC_SFML)

#The grid
Grid.o: Grid.cpp Grid.hpp Colors.o LehmerRandom.o
	$(CC) Grid.cpp -c $(INC_SFML)

Colors.o: Colors.cpp Colors.hpp
	$(CC) Colors.cpp -c $(INC_SFML)

LehmerRandom.o: LehmerRandom.cpp LehmerRandom.hpp
	$(CC) LehmerRandom.cpp -c
################

clean:
	rm *.o test