#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"
#include <stdlib.h>  
#include <time.h>

//set up the console.   Don't modify this line!
Console out;

int main()
{
	//Set up the window.  Don't edit these two lines
    terminal_open();
  	terminal_set(SETUP_MESSAGE);

/*
    The code below provides a skeleton of the game play.  You will need to
    write code for setting up the game board, and playing the game itself.
    You can modify the code given as needed.
    
    Call the functions that you have written in the game play file, and that
    you have added to the Actor class.
*/	
	//start timer for pseudo random number generator
	srand (time(NULL));

    //make the player
    //randomizes player spawn to be 0 <= x <= 40 and 0 <= y <= 5
    int playerX = rand() % 41;
    int playerY = rand() % 6;
	Actor player(PLAYER_CHAR, playerX, playerY);  // you probably don't want to start in the same place each time

	//make the monster
	//randomizes player spawn to be 40 <= x <= 79 and 14 <= x <= 20
	int monsterX = rand() % 41 + 39;
    int monsterY = rand() % 6 + 15;
	Actor monster(MONSTER_CHAR, monsterX,monsterY, player);

    // Declare the array that will hold the game board "map"
  	int map[NUM_BOARD_Y][NUM_BOARD_X] = {0};
/*
    Initiallize locations in the game board to have game features.  What if you
    have man things to add to the game board?  Should you use a loop?  Does it
    make sense to store this information in a file?  Should this code be a
    function as well?
*/	
	//generates 15 random walls in random locations and random length
	for (int i = 0; i < 15; i++)
	{
		//randomizes the start of a wall to be a random spot on the map
		int wallStartX = rand() % NUM_BOARD_X;
		int wallStartY = rand() % NUM_BOARD_Y;
		//randomizes the length of the wall to be from 4-9
		int wallLength = rand() % 5 + 5;
		//randomizes whether the wall is vertical or negative
		bool verticalWall = rand() % 2;
		//randomizes the direction(positive or negative) the wall extends from the start of the wall
		int direction = 1;
		if (rand() %2 == 1)
			direction = -1;
		//establishes the wall based on generated information
		if (direction == -1)
		{
			if (verticalWall)
			{
				for (int y = wallStartY; y > wallStartY - wallLength; y--)
				{
					//test if part of wall is not in bounbs or not
					if (y >= 0 && y <NUM_BOARD_Y)
						map[y][wallStartX] = SHALL_NOT_PASS;		
				}		
			}
			else
			{	
				for (int x = wallStartX; x > wallStartX - wallLength ; x--)
				{
					//test if part of wall is not in bounbs or not
					if (x >= 0 && x < NUM_BOARD_X)
						map[wallStartY][x] = SHALL_NOT_PASS;		
				}
	    			
			}	
		}
		else
		{
			if (verticalWall)
			{
				for (int y = wallStartY; y < wallStartY + wallLength; y++)
				{
					//test if part of wall is not in bounbs or not
					if (y > -1 && y <NUM_BOARD_Y)
						map[y][wallStartX] = SHALL_NOT_PASS;		
				}		
			}
			else
			{	//test if part of wall is not in bounbs or not
				for (int x = wallStartX; x < wallStartX + wallLength ; x++)
				{
					if (x > -1 && x < NUM_BOARD_X)
						map[wallStartY][x] = SHALL_NOT_PASS;		
				}
	    			
			}	
		}
			
	}
	//generate random safe zone with 40 <= x <= 79 and 14 <= y <= 20
	int safeZoneX = rand() % 41 + 39;
    int safeZoneY = rand() % 6 + 15;
	map[safeZoneY][safeZoneX] = WINNER;
  	
    // Call the function to print the game board
  	
	// Printing the instructions
    out.writeLine("Escape the Goose! " + monster.get_location_string());
	out.writeLine("Use the arrow keys to move");
	out.writeLine("If the goose catches you, you lose!");
	out.writeLine("Be careful! Sometimes the goose can jump through walls!");
	
	//randomly generate 3 teleporters
	for (int i = 0; i < 3; i++)
	{
		int teleportX = rand() % NUM_BOARD_X;
		int teleportY = rand() % NUM_BOARD_Y;
		map[teleportY][teleportX] = TELEPORTER;
	}

/*
    This is the main game loop.  It continues to let the player give input
    as long as they do not press escape or close, they are not captured by
    the goose, and they didn't reach the win tile
*/

/*
    All key presses start with "TK_" then the character.  So "TK_A" is the "a"
    key being pressed.
*/
    int keyEntered = TK_A;  // can be any valid value that is not ESCAPE or CLOSE
    bool game_won = false;
    bool hit_by_spitball = false;
    
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE 
                    && !captured(player,monster) && !game_won && !hit_by_spitball)
	{
		printBoard(map);
	    // get player key press
	    keyEntered = terminal_read();
        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player, you can modify this function
    	    movePlayer(keyEntered, player, map);
    	    
    	    //moves the spitball and resets it if needed
			monster.move_spitball(player);
			
            // call the goose's chase function
            follow (player, monster, map);
            // test if player is hit by spitball
            hit_by_spitball = hitBySpitball(player, monster.get_spitball());
            //test if player has reached safezone
			game_won = test_win (player, map);
			//test if the player stepped on a teleporter
			teleport (player, map);

        }
  	}

    if (keyEntered != TK_CLOSE)
    {
      	//once we're out of the loop, the game is over
        out.writeLine("Game has ended");
    
        // output why:  did the goose get us, or did we get to the win location?
	
    	// Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it  
    terminal_close();
}


//cplusplus.com, "rand," cplusplus.com, [Online]. Available: https://www.cplusplus.com/reference/cstdlib/rand/. [Accessed 23 11 2020].
