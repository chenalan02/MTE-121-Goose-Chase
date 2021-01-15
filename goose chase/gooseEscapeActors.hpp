#ifndef GOOSE_ESCAPE_ACTORS
#define GOOSE_ESCAPE_ACTORS

//#include <cmath>
//#include <BearLibTerminal.h>
//#include "gooseEscapeUtil.hpp"
#include<string>
using namespace std;

/*
    Modify this class to contain more characteristics of the "actor".  Add
    functions that will be useful for playing the game that are specific to
    the Actor.
    
    Feel free to add additional Classes to your program.
*/

/* 
    Going further:  Learn the other syntax for implementing a class that is
    more appropriate for working with multiple files, and improve the class code.
*/
//class for a spitball meant to be shot by the goose at the player





//class definitions in gooseEscapeActors.cpp
class Actor;

//spitball object are to be shot at the player by the goose
//once a spitball goes out of bounds a new one is shot
class Spitball
{
	private:
		int posX;
		int posY;
		int velocityX;
		int velocityY;
		
	public:
		
		void resetSpitball(Actor & goose, Actor & player);
		
		int get_velocityX() const;
	
		int get_velocityY() const;
	
		int get_x() const;
		
		int get_y() const;
		
		void Move(Actor & goose, Actor & player);
};


class Actor
{
	//actor class has a spitball member that does nothing unless activated
  private:
    int actorChar;      
    int location_x, location_y;
    Spitball spitball;

  public:
  	
    Actor();
        
    Actor(char initPlayerChar, int x0, int y0);
	//constructor used for monster in main activates spitball
    Actor(char initPlayerChar, int x0, int y0, Actor & player);
    
    Spitball get_spitball() const;
	
    int get_x() const;
    
    int get_y() const;
    
    string get_location_string() const;
    
    void change_char(char new_actor_char);

    bool can_move(int delta_x, int delta_y) const;

    bool update_location(int delta_x, int delta_y);
    
    void put_actor() const;
    
    void move_spitball(Actor & player);
};

#endif
