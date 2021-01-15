
#include "gooseEscapeActors.hpp"

#include <cmath>
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"



//resets or creates to spitball that is 3 tiles in length to be shot at the player
void Spitball::resetSpitball(Actor & goose, Actor & player)
{
	int displacementX, displacementY;

	displacementX = player.get_x() - goose.get_x();
	displacementY = player.get_y() - goose.get_y();
	
	//velocity is set to a magnitude of 3 with direction depending on which displacement is further
	//first 2 if statements for cases where displacements are 0 so the calculations for velocity dont fail
	if (displacementX == 0)
	{
		velocityY = 3*(displacementY/abs(displacementY));
		velocityX = 0;
	}
			
	else if (displacementY == 0)
	{	
		velocityX = 3*(displacementX/abs(displacementX));
		velocityY = 0;
	}
	
	else if (abs(displacementX) > abs(displacementY))
	{
		velocityX = 3*(displacementX/abs(displacementX));
		velocityY = 0;
	}
				
	else
	{
		velocityY = 3*(displacementY/abs(displacementY));
		velocityX = 0;
	}
	//sets the position in relation to the goose (in front)	
	posX = goose.get_x() + velocityX;
	posY = goose.get_y() + velocityY;

	terminal_put(posX, posY, int('*'));
   	terminal_refresh();
}



int Spitball::get_velocityX() const
{
	return velocityX;
}

int Spitball::get_velocityY() const
{
	return velocityY;
}

int Spitball::get_x() const
{
	return posX;
}

int Spitball::get_y() const
{
	return posY;
}

//method to move the spitball every frame and redraw it in the terminal
//note that the spitball is 3 tiles in length that depend on direction it is moving
void Spitball::Move(Actor & goose, Actor & player)
{
	//clears the previous spitball based on direction it is moving
	if (velocityX == 0)
	{
		for (int i = 0; i < 3; i++)
			terminal_clear_area(posX, posY-i*(velocityY/3), 1, 1);
	}
	else
	{
		for (int i = 0; i < 3; i++)
			terminal_clear_area(posX-i*(velocityX/3), posY, 1, 1);
	}
	//moves the spitball 
	posX += velocityX;
	posY += velocityY;
	
	//detects if the spitball reached out of bounds and resets it if it has
	if (posY < 0 || posY > NUM_BOARD_Y - 1 || posX < 0 || posX > NUM_BOARD_X - 1)
	{
		resetSpitball(goose, player);
	}
		
	//redraws the spitball in terminal depending on velocity
	if (velocityX == 0)
	{
		for (int i = 0; i < 3; i++)
			terminal_put(posX, posY-i*(velocityY/3), int('*'));
	}
	else
	{
		for (int i = 0; i < 3; i++)
			terminal_put(posX-i*(velocityX/3), posY, int('*'));
	}
	terminal_refresh();
}


Actor::Actor()
{
    actorChar = int('A');
    location_x = MIN_SCREEN_X;
    location_y = MIN_SCREEN_Y;
    put_actor();
}
    
Actor::Actor(char initPlayerChar, int x0, int y0)
{
    change_char(initPlayerChar);
    location_x = MIN_SCREEN_X;
    location_y = MIN_SCREEN_Y;
    update_location(x0,y0);
}
//constructor for monster/goose to utilize spitball member
Actor::Actor(char initPlayerChar, int x0, int y0, Actor & player)
{
    change_char(initPlayerChar);
    location_x = MIN_SCREEN_X;
    location_y = MIN_SCREEN_Y;
    update_location(x0,y0);

    spitball.resetSpitball(*this, player);
}

Spitball Actor::get_spitball() const
{
	return spitball;
}

int Actor::get_x() const
{
    return location_x;
}

int Actor::get_y() const
{
    return location_y;
}

string Actor::get_location_string() const
{
    char buffer[80];
    itoa(location_x,buffer,10);
    string formatted_location = "(" + string(buffer) + ",";
    itoa(location_y,buffer,10);
    formatted_location += string(buffer) + ")";
    return formatted_location;
}

void Actor::change_char(char new_actor_char)
{
    actorChar = min(int('~'),max(int(new_actor_char),int(' ')));
}

bool Actor::can_move(int delta_x, int delta_y) const
{
    int new_x = location_x + delta_x;
    int new_y = location_y + delta_y;

    return new_x >= MIN_BOARD_X && new_x <= MAX_BOARD_X
      && new_y >= MIN_BOARD_Y && new_y <= MAX_BOARD_Y;
}

bool Actor::update_location(int delta_x, int delta_y)
{
    if (can_move(delta_x, delta_y))
    {
        terminal_clear_area(location_x, location_y, 1, 1);
        location_x += delta_x;
        location_y += delta_y;
        put_actor();
        return true;
    }
    return false;
}

void Actor::put_actor() const
{
    terminal_put(location_x, location_y, actorChar);
    terminal_refresh();
}

//function to move spitball
void Actor::move_spitball(Actor & player)
{
	spitball.Move((*this), player);
}
