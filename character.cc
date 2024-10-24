#include "character.h"
#include "defs.h"
#include <cmath>
using namespace std;

Character::Character(Coords posn, string type, int health, int attack, int defense): Entity{posn}, health{health}, attack{attack}, defense{defense} {
	this->type = type;
}

int Character::getHP() const{
	return health;
}

int Character::getATK() const{
	return attack;
}

void Character::move(string dir) {
	Coords nc = posn;
	if(dir == "no") nc.y--;
    else if(dir == "so") nc.y++;
    else if(dir == "ea") nc.x++;
    else if(dir == "we") nc.x--;
    else if(dir == "ne") {nc.y--; nc.x++;}
    else if(dir == "nw") {nc.y--; nc.x--;}
    else if(dir == "se") {nc.y++; nc.x++;}
    else {nc.y++; nc.x--;}

	posn = nc;
}

void Character::receiveDMG(int playerATK) {

	int damage = ceil((100) * playerATK / (100 + defense));
	health -= damage;
	if ((health) < 0) health = 0;
	cout<<getType()<<" damaged: "<<damage<<" health: "<<health<<endl;
}

bool Character::isItem() {
	return false; // Always returns false as we are in the character subclass so all of the objects would be characters.
}

bool Character::isSpawner() {
	return false; // Always returns false excpet the spawner subclass where it is set to true.
}

shared_ptr<Item> Character::getDrop() {
	int random_num = rand()%2;
	Effect e = {0, 0, 0, 0, 0};
	string t = "";

	if(random_num == 0) {
		t = "G Small Gold";
		e.money = 1;
	} else {
		t = "G Normal Gold";
    e.money = 2;
	}

	return make_shared<Item>(posn, t, e);
}

string Character::getType() const{
	return type;
}

