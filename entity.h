#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <string>
#include "defs.h"
#include "memory"
using namespace std;

class Entity {

	protected:
		Coords posn;

	public:
		Entity(Coords posn); // Default constructor
		virtual ~Entity() = default;
		virtual string getType() const = 0; // Gets type of character or item.
		Coords getLocation() const; // Gets the location coordinates for the entity.
		Coords setLocation(int x, int y); // Changes the location of the entity.
		virtual bool isItem() = 0; // Returns true if the object belongs to item class.
};

#endif
