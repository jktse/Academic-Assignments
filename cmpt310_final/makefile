all: Reversi PvR HvR PvH 

Reversi: main.o reversi.o ai.o
		g++ -Wall -std=c++11 -o Reversi main.o reversi.o ai.o

PvR: PvR.o reversi.o ai.o
		g++ -Wall -std=c++11 -o PvR PvR.o reversi.o ai.o

PvH: PvH.o reversi.o ai.o
		g++ -Wall -std=c++11 -o PvH PvH.o reversi.o ai.o

HvR: HvR.o reversi.o ai.o
		g++ -Wall -std=c++11 -o HvR HvR.o reversi.o ai.o

HvR.o: HvR.cpp
		g++ -c HvR.cpp

PvH.o: PvH.cpp
		g++ -c PvH.cpp

PvR.o: PvR.cpp
		g++ -c PvR.cpp

main.o: main.cpp
		g++ -c main.cpp

ai.o: ai.h ai.cpp reversi.o
		g++ -c ai.cpp

reversi.o: reversi.cpp reversi.h
		g++ -c reversi.cpp

clean:
		rm -f Reversi PvR HvR PvH *.o