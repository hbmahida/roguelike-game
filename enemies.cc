#include "enemies.h"
using namespace std;

// Human
Human::Human(Coords c): Character(c, "H Human", 140, 20, 20) {}

shared_ptr<Item> Human::getDrop() {
	Effect e = {0, 0, 0, 0, 0};
	e.money = 4;

	return make_shared<Item>(posn, "G 2 Normal Piles", e);
}


// Elf
Elf::Elf(Coords c): Character(c, "E Elf", 140, 30, 10) {}


// Orc
Orc::Orc(Coords c): Character(c, "O Orc", 180, 30, 25) {}


// Dwarf
Dwarf::Dwarf(Coords c): Character(c, "W Dwarf", 100, 20, 30) {}


// Halfling
Halfling::Halfling(Coords c): Character(c, "L Halfling", 100, 15, 20) {}

void Halfling::receiveDMG(int playerATK) {
	int random = rand()%2;
	if (random == 0) {
		int damage = ceil((100) * playerATK / (100 + defense));
		health -= damage;
		if ((health) < 0) health = 0;
	}
}


// Merchant
Merchant::Merchant(Coords c, vector<shared_ptr<Item>> temp): Character(c, "M Merchant", 30, 70, 5) {
	while(!temp.empty()) {
		trades.push_back(shared_ptr<Item>(temp.back().get()));
		temp.pop_back();
	}
}

bool Merchant::isHit() {
	return hit;
}

vector<shared_ptr<Item>> Merchant::getTrades() {
	if (hit) { // Merchant is now hostile so wouldn't trade.
		return {}; 
	} else {
		return trades; // Merchant is neutral and can trade.
	}
}

int Merchant::getATK() const{
	if (!hit) return 0;
	else return attack;
}

void Merchant::receiveDMG(int playerATK) {
	hit = true;

	int damage = ceil((100) * playerATK / (100 + defense));
	health -= damage;
	if ((health) < 0) health = 0;
}

shared_ptr<Item> Merchant::getDrop() {
	Effect e = {0,0,0,4,0};

	return make_shared<Item>(posn, "G Merchant Hoard", e,false,1,0);
}


// Dragon
Dragon::Dragon(Coords c, Item *hoard): Character(c, "D Dragon", 150, 20, 20), hoard{hoard} {}

shared_ptr<Item> Dragon::getDrop() {
	if (health == 0) hoard->unlock();
	return shared_ptr<Item>(nullptr);
}

Item *Dragon::getHoard() {
	return hoard;
}

void Dragon::move(string dir) {
	return; // Dragon is stationary so doesn't move.
}


// Spitter
Spitter::Spitter(Coords c): Character(c, "S Spitter", 120, 10, 30) {}

shared_ptr<Item> Spitter::getDrop() {
	Coords c = posn;
	string type = "B SpitBall";
	Effect e = {0, 0, 0, 0, 0};
	e.money = 5;
	e.passiveDMG = -15;
	e.hp = -40;

	return shared_ptr<Item>(new Item(posn, type, e));
}


// Child
Child::Child(Coords c, string type, int HP, int ATK, int DEF, Spawner *parent): Character(c, type, HP, ATK, DEF), parent{parent} {}

void Child::die() {
	health = 0;
	parent->deleteChild(this);
}

void Child::notifySpawnerDeath() {
	health = 0;
}


// Vine
Vine::Vine(Coords c): Child(c, "V Vine", 100, 0, 10, parent) {}


// Root
Root::Root(Coords c): Child(c, "R Root", 50, 10, 0, parent) {}


// Spawner
Spawner::Spawner(Coords c, string type, int HP, int ATK, int DEF): Character{c, type, HP, ATK, DEF} {}

void Spawner::addChild(Child *child) {
	children.emplace_back(child);
}

void Spawner::deleteChild(Child *child) {
	for (int i=0; i<children.size();++i) {
		if (child == children[i]) {
			children.erase(children.begin()+i);
		}
	}
}

void Spawner::notifyDeath() {
	if (health == 0) {
		while (children.size() != 0) {
			children.back()->notifySpawnerDeath();
			children.pop_back();
		}
	}
}


bool Spawner::isSpawner() {return true;}


// VineHead
VineHead::VineHead(Coords c): Spawner(posn, "I VineHead", 200, 0, 0) {}

shared_ptr<Child> VineHead::getNewChild() {
	// Selects whether to grow vines in east or south direction as VineHead spawns on top-left corner of room.
	int x = 0;
	int y = 0;

	Coords new_posn = {x,y,-1}; 

	int highest_x = 0;
	int highest_y = 0;
	for (int i=0; i<children.size(); ++i) {
		Coords loc = children[i]->getLocation();

		if (loc.x > highest_x) highest_x = loc.x;
		if (loc.y > highest_y) highest_y = loc.y;
	}

	int random = rand()%2;
	if (random == 0) {
		x += (highest_x + 1); // East
	} else {
		y += (highest_y + 1); // South
	}

	return make_shared<Vine>(new_posn);
}

int VineHead::getFreq() {
	return 2;
}


// RootHead
Mandrake::Mandrake(Coords c): Spawner(posn, "A Mandrake", 250, 0, 0) {}

int Mandrake::getFreq() {
	return -5;
}

shared_ptr<Child> Mandrake::getNewChild() {
	Coords position = {-1,-2,-3}; // Purposely given invalid Coords structure for spawn mechanics.

	return make_shared<Root>(position);
}


