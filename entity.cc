#include "entity.h"

Entity::Entity(Coords posn): posn{posn} {}

Coords Entity::getLocation() const{
	return posn;
}

Coords Entity::setLocation(int x, int y) {
	posn.x = x;
	posn.y = y;
	return posn;
}
