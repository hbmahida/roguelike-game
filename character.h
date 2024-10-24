#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <string>
#include "item.h"
#include <memory>
#include "entity.h"
using namespace std;

class Character: public Entity {
	string type;

	protected:
		int attack, health, defense;

	public:
		Character(Coords posn, string type, int health, int attack, int defense); // Default Constructor
		~Character() = default;
		int getHP() const; // Gets for the character HP.
		virtual int getATK() const; // Gets the character Attack stats.
		virtual void move(string dir); // Moves the character in the specified direction dir.
		virtual void receiveDMG(int playerATK); // Damages the character according to the damage 
							// inflicted by the player.
		virtual shared_ptr<Item> getDrop(); // Relieves the character of its ownership of the dropped item (only treasure in base game) on death, called by the entity updater in chamber (location will be set by caller).
		bool isItem() override; // Returns true if entity type is item.
		virtual bool isSpawner(); // Important for update logic. Returns true if the character type is a spawner.
		bool isTrader() {return false;}
		string getType() const override;
};

#endif
