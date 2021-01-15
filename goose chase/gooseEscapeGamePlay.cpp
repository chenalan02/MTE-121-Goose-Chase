#include <iostream>
#include <cmath>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"
#include <stdlib.h>  
#include <time.h>

#include <iostream>
using namespace std;

extern Console out;
/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner.  So it means the coordinate axes are:
---------------->   x direction
|
|
|
|
|
V

y direction 
*/
	
/*
    Print the game world
    
    The functions should draw characters to present features of the game
    board, e.g. win location, obstacles, power ups
*/

// print the game board function
void printBoard(int map[NUM_BOARD_Y][NUM_BOARD_X])
{
	for (int y = 0; y < NUM_BOARD_Y; y++)
	{
		for (int x = 0; x < NUM_BOARD_X; x++)
		{
			if (map[y][x] != EMPTY)
			{
				if (map[y][x] == SHALL_NOT_PASS)
					terminal_put(x,y, WALL_CHAR);
				else if (map[y][x] == WINNER)
					terminal_put(x,y, WIN_CHAR);
				else
					terminal_put(x,y, TELEPORTER_CHAR);
			}
		}
	}
	terminal_refresh();
}

/*
    Do something when the goose captures the player
    
    If you want to attack or something else, this is the function you 
    need to change.  For example, maybe the two touch each other and
    then fight.  You could add a health to the Actor class that is
    updated.  Run, use weapons, it's up to you!
*/

bool captured(Actor const & player, Actor const & monster)
{
    return (player.get_x() == monster.get_x() 
         && player.get_y() == monster.get_y());
}

//hit detection for spitballs
bool hitBySpitball(Actor const & player, Spitball const & spitball)
{
	bool hit = false;
	//if the velocity of the spitball in the x axis is 0, it must be moving in the y axis
	if (spitball.get_velocityX() == 0)
	{	//iterate through the position of the spitball and the next 2 positions behind it which depends on direction of veloctity
		//if the player is hit by any of these 3 positions it counts as a hit
		for (int i = 0; i < 3; i++)
		{	
			if(player.get_x() == spitball.get_x() && player.get_y() == spitball.get_y()-i*(spitball.get_velocityY()/3))
			{
				hit = true;
			}

		}
	}
	else
	{	//if the velocity of the spitball in the x axis is not 0, it must be moving in the x axis only
		for (int i = 0; i < 3; i++)
		{	
			//iterate through the position of the spitball and the next 2 positions behind it which depends on direction of veloctity
			//if the player is hit by any of these 3 positions it counts as a hit
			if(player.get_x() == spitball.get_x()-i*(spitball.get_velocityX()/3) && player.get_y() == spitball.get_y())
			{
				hit = true;
			}

		}
	}
	cout << endl;
	return hit;		
}

/*
    Move the player to a new location based on the user input.  You may want
    to modify this if there are extra controls you want to add.
    
    All key presses start with "TK_" then the character.  So "TK_A" is the a
    key being pressed.
    
    A look-up table might be useful.
    You could decide to learn about switch statements and use them here.
*/

void movePlayer(int key, Actor & player, int map[NUM_BOARD_Y][NUM_BOARD_X])
{
    int yMove = 0, xMove = 0;
    if (key == TK_UP)
        yMove = -1;
    else if (key == TK_DOWN)
        yMove = 1;
    else if (key == TK_LEFT)
        xMove = -1;
    else if (key == TK_RIGHT)
        xMove = 1;
        
    if (player.can_move(xMove, yMove) 
      && map[player.get_y()+yMove][player.get_x()+xMove] != SHALL_NOT_PASS)
        player.update_location(xMove, yMove);
}

void follow (Actor const & player, Actor & monster, int map[NUM_BOARD_Y][NUM_BOARD_X])
{
	//finds the x and y displacements
    int x_displacement, y_displacement;
    x_displacement = player.get_x() - monster.get_x();
    y_displacement = player.get_y() - monster.get_y();
    	
    //create preference to move in the axis with the longer displacement
    bool x_is_better_direction = abs(x_displacement) > abs(y_displacement);
    // boolean to not move the monster if the player moves into the monster
    bool in_player = x_displacement == 0 && y_displacement == 0;
	int xMove = 0;
	int yMove = 0;
	// random default moves if displacement is 0
	int best_x_move = 1;
	int best_y_move = 1;
	
	srand (time(NULL));
	int randomDefaultDirection = rand()%2;
	if (randomDefaultDirection == 1)
	{
		best_x_move = -1;
		best_y_move = -1;
	}
		
	// the best move determined by the direction of the player from the monster 
	if (x_displacement != 0)
		best_x_move = x_displacement/abs(x_displacement);
	if (y_displacement != 0)
		best_y_move = y_displacement/abs(y_displacement);

	if (x_is_better_direction && !in_player)
	{
		xMove = best_x_move;
		// if the monster is not blocked from moving towards the player in the x axis, move that way
		if (monster.can_move(xMove, yMove) && map[monster.get_y()+yMove][monster.get_x()+xMove] != SHALL_NOT_PASS)
			monster.update_location(xMove, yMove);
		// moves in the y axis if blocked from the prefered direction
		else
		{	//gives random 1/20 chance the goose jumps over a wall
			if (rand()%20 == 0)
				monster.update_location(xMove*2, yMove);
			else
			{
				xMove = 0;
				yMove = best_y_move;
				//moves unless blocked
				if (monster.can_move(xMove, yMove) && map[monster.get_y()+yMove][monster.get_x()+xMove] != SHALL_NOT_PASS)
					monster.update_location(xMove, yMove);
			}
			
		}
	}	
	else if (!x_is_better_direction && !in_player)
	{	// if the monster is not blocked from moving towards the player in the y axis, move that way
		yMove = best_y_move;
		if (monster.can_move(xMove, yMove) && map[monster.get_y()+yMove][monster.get_x()+xMove] != SHALL_NOT_PASS)
			monster.update_location(xMove, yMove);
		// moves in the y axis if blocked from the prefered direction
		else
		{	//gives random 1/20 chance the goose jumps over a wall
			if (rand()%20 == 0)
				monster.update_location(xMove, yMove*2);
			else
			{
				yMove = 0;
				xMove = best_x_move;
				//moves unless blocked
				if (monster.can_move(xMove, yMove) && map[monster.get_y()+yMove][monster.get_x()+xMove] != SHALL_NOT_PASS)
					monster.update_location(xMove, yMove);
			}
			
		}
	}		
}

bool test_teleport (Actor const & player, int map[NUM_BOARD_Y][NUM_BOARD_X])
{
	if (map[player.get_y()][player.get_x()] == TELEPORTER)
		return true;
}

void teleport (Actor & player, int map[NUM_BOARD_Y][NUM_BOARD_X])
{
	if (test_teleport(player, map))
	{	
		bool done = false;
		while (!done)
		{
			srand (time(NULL));
			int newX = rand() % NUM_BOARD_X;
			int newY = rand() % NUM_BOARD_Y;
			int deltaX = newX - player.get_x();
			int deltaY = newY - player.get_y();
			
			if (player.update_location(deltaX, deltaY))
				done = true;
		}	
	}
}

// returns if the player is on the winner tile
bool test_win (Actor const & player, int map[NUM_BOARD_Y][NUM_BOARD_X])
{
	if (map[player.get_y()][player.get_x()] == WINNER)
		return true;
	else
		return false;
}
/*
    What other functions do you need to make the game work?  What can you
    add to the basic functionality to make it more fun to play?
*/
