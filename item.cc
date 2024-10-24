#include "item.h"
using namespace std;

Item::Item(Coords c, string type, Effect e, bool lock, int dur, int price): Entity(c), effect{e}, isLocked{lock}, duration{dur}, price{price} {
	this->type = type;
}

int Item::getDuration() const{
	return duration;
}

bool Item::isItem() {
	return true; // Always true as we are under the Item subclass so all the objects under that would have the type item.
}

Effect Item::getEffect() const{
	return effect;
}

int Item::getPrice() const{
	return price;
}

string Item::getType() const{
	return type;
}

void Item::tick() {
	duration--;
}

// For Dragon
bool Item::locked() {
	return isLocked;
}

void Item::unlock() {
	isLocked = false;
}
