#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string>
#include "item.h"
#include "defs.h"
#include "gamemap.h"
using namespace std;

class Player {
    int onFloor = 0;
    protected:
        int MAX_HP; // useful to check for vampire
        vector<shared_ptr<Item>> items = {};
        int money = 0;
        int HP, ATK, DEF;
        Coords c = {0, 0, 0};
        Floor* floor = nullptr;
        string playerAction = "Player character has spawned.";
        string entityAction = ""; 

    public:
        Player(int HP, int ATK, int DEF);
        void setMAX_HP(int max_HP) { MAX_HP = max_HP; }
        bool check_floor();
        bool move(string dir);
        virtual bool attack(string dir);
        virtual void receiveEntityActions(); // takes effects from entity and produces results
        void receiveEffect(Effect effect); // specific to healer
        virtual void Racetype() = 0;
        void printStatus(); // print in specific order details of the player
        void getRender(vector<char> &v); // call render of floor
        virtual void tickItems(); //applies items and refreshes lifespans of each item
        bool isDead();
        bool isComplete();
        void setFloor(Floor *floor, int n);
        bool trade(string dir);
        void setCoords(Coords c);
        void quit();
        int getGold();
};

#endif
