#ifndef GAMEMAP_H
#define GAMEMAP_H
#include "defs.h"
#include "entity.h"
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include "item.h"
#include "enemies.h"
#include "character.h"

using namespace std;
//game map modules (Floor and Chamber)
class Chamber {
    vector<bool> limits;
    vector<shared_ptr<Entity>> entities;
    int hsize, vsize;
    int updates = 0; //used for some spawn mechanics
    public:
        Chamber(vector<bool> l, int hs, int vs);
        Coords getMove(Coords c, string dir, vector<Coords> &ppos);
        void inRange(vector<Entity*> &v, Coords c);
        Entity *getTarget(Coords c, string dir, bool isAttack);
        void updateEntities(vector<Coords> &ppos);
        bool addEntity(shared_ptr<Entity> e, vector<Coords> &ppos);
        bool spawnEntity(shared_ptr<Entity> e, vector<Coords> &ppos);
        void getAllEntities(vector<Entity*> &v);
        shared_ptr<Item> pickup(Entity *i);
};


class Floor {
    int hsize;
    int vsize;
    vector<char> map; //it's now a vector because smart pointers wont behave
    vector<Chamber> chambers; //this is not done with pointers because chambers will only be referenced within the floor class
    Coords stairs = {0, 0, 0};
    vector<int> roomMap; //keeps track of which chamber a cell is in
    vector<Coords> ppos; //keeps track of player positions (the map doesn't need to know anything else about the player)
    int cs; //haha funny
            //which chamber the players spawn in
    public:
        Floor(string file, bool plain, bool dlc);
        Coords addPlayer(); //adds a player to the ppos vector, which is used to check for conflicts between players (not these are ONLY coords because nothing else is needed on the map)
        Coords getMove(Coords c, string dir);
        void inRange(vector<Entity*> &v, Coords c);
        Entity *getTarget(Coords c, string dir, bool isAttack);
        void updateEntities();
        bool complete(const Coords &c);
        //note: e will NOT point to the entity afterwards
        bool addEntity(shared_ptr<Entity> e, int cham); //use when adding entities without knowing specific coordinates
        bool spawnEntity(shared_ptr<Entity> e); //use when everything in the entity is already set
        void getRender(vector<char> &v); //we are just pushing stuff directly into the render because vector copy dont work
        shared_ptr<Item> pickup(Entity *i); //this transfers ownership to whoever calls it (the item is taken out of the entity list)
        int getH() {return hsize;}
        int getV() {return vsize;}
};

#endif
