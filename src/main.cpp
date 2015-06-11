#include <iostream>

#include "game.h"

int main()
{
	Game mygame;

	mygame.initialize();

	mygame.run();

	mygame.shutdown();

    return 0;
}//main
