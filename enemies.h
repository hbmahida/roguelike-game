#ifndef ENEMIES_H
#define ENEMIES_H

#include <iostream>
#include <string>
#include "item.h"
#include <vector>
#include <memory>
#include "character.h"
#include <cmath>
using namespace std;

// Base Game Characters

// Human
class Human: public Character {
	public:
		Human(Coords c);
		~Human() = default;
		shared_ptr<Item> getDrop() override;
};


// Elf
class Elf: public Character {
	public:
		Elf(Coords c);
		~Elf() = default;
};


// Dwarf
class Dwarf: public Character {
	public:
		Dwarf(Coords c);
		~Dwarf() = default;
};


// Orc
class Orc: public Character {
	public:
		Orc(Coords c);
		~Orc() = default;
};


// Halfling
class Halfling: public Character {
	public:
		Halfling(Coords c);
		void receiveDMG(int playerATK) override;
		~Halfling() = default;
};


// Merchant
class Merchant: public Character {

	static inline bool hit = false;
	vector<shared_ptr<Item>> trades;

	public:
		Merchant(Coords c, vector<shared_ptr<Item>> temp);
		bool isHit(); // Gets the hit status of a merchant.
		vector<shared_ptr<Item>> getTrades(); // DLC Component
																// Gets the prices and available potions from Merchant.
																//if empty, then it's the base game.
		int getATK() const override;
		void receiveDMG(int playerATK) override;
		shared_ptr<Item> getDrop() override;
		bool isTrader() {return true;}
		~Merchant() = default;
};


// Dragon
class Dragon: public Character {
	Item *hoard;

	public:
		Dragon(Coords c, Item *hoard);
		~Dragon() = default;
		shared_ptr<Item> getDrop() override;
		void move(string dir) override;
		Item *getHoard(); // Returns the pointer to the hoard.
};


// DLC Characters

//Spitter
class Spitter: public Character {

	public:
		Spitter(Coords c);
		~Spitter() = default;
		shared_ptr<Item> getDrop() override;
};


// Child and Spawner follow Observer Pattern.

class Child;

// Spawner
class Spawner: public Character {
	protected:
		vector <Child *> children;

	public:
		Spawner(Coords c, string type, int HP, int ATK, int DEF);
		virtual ~Spawner() = default;
		virtual shared_ptr<Child> getNewChild() = 0; //gets a child pointer for spawning (spawning will depend on coords validity)
										 //if this returns anything other than the spawner's chamber
										 //in the chamber section for its coords, then spawning will be handled by the chamber, otherwise only valid xy will result in spawning
		void addChild(Child *child); // Adds the given child to children.
		void deleteChild(Child *child); // Removes the given child from children. Called when a Child dies.
		void notifyDeath(); //call this method when health reaches 0, so everything can be removed at once the next turn
		bool isSpawner() override; // Always true because the code is under Spawner subclass of Character.
		virtual int getFreq() = 0; //returns the spawn frequency (if positive, then it is a set tick number, if negative, it would represent a 1/-n probability)
							//for negative, -6 would mean a 1-in-6 probability
							//for positive, 7 would mean it spawns every 7 ticks
};


// Child
class Child: public Character {
	Spawner *parent;

	public:
		Child(Coords c, string type, int HP, int ATK, int DEF, Spawner *parent);
		virtual ~Child() = default;
		void die(); // Notifies the spawner that it has died. Called when child is killed. 
		void notifySpawnerDeath(); //Notifies the child that the spawner has died so it will also die.
};


// VineHead : produces Vine(s)
class VineHead: public Spawner {

	public:
		VineHead(Coords c);
		~VineHead() = default;
		shared_ptr<Child> getNewChild() override; 
		int getFreq() override;
};


// Vine
class Vine: public Child {
	VineHead *parent;

	public:
		Vine(Coords c);
		~Vine() = default;
};


// Mandrake : produces Root(s)
class Mandrake: public Spawner {

	public:
		Mandrake(Coords c);
		~Mandrake() = default;
		shared_ptr<Child> getNewChild() override;
		int getFreq() override;
};


// Root
class Root: public Child {
	Mandrake *parent;

	public:
		Root(Coords c);
		~Root() = default;
};

#endif
