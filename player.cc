#include <vector>
#include <string>
#include "item.h"
#include "defs.h"
#include "gamemap.h"
#include <sstream>
#include "player.h"
#include <cmath>
#include <cassert>
using namespace std;

Player::Player(int HP, int ATK, int DEF) : HP{HP}, ATK{ATK}, DEF{DEF} {}

bool Player::move(string dir) { 
  Coords nc = floor->getMove(c, dir); 
  if (c == nc) { 
    Item *itemOrNot = static_cast<Item *>(floor->getTarget(c, dir, false));
    if (itemOrNot) { 
      string name = itemOrNot->getType().substr(2);
      items.emplace_back(floor->pickup(static_cast<Item*>(itemOrNot))); 
      nc = floor->getMove(c, dir);
      c = nc;
      playerAction = "PC used "+name+".";
      return true;
    }
  }
  if(c != nc) {
    c = nc;
    playerAction = "PC moved "+dir+".";
    return true;
  }
  playerAction = "PC move FAILED!";
  return false;
}

bool Player::attack(string dir) { 
  Entity *entity = floor->getTarget(c, dir, true);
  if (entity) {  
    Character *enemy = static_cast<Character*>(entity);
    int old_HP = enemy->getHP();
    enemy->receiveDMG(ATK);
    int new_HP = enemy->getHP();
    ostringstream damage, health;
    damage << (old_HP - new_HP);
    health << new_HP;
    playerAction = "PC deals " + damage.str() + " to " + entity->getType()[0] + " (" + health.str() + " HP)"; 
    return true;
  }
  return false;
}

void Player::receiveEntityActions() {
  vector<Entity*> entities;
  floor->inRange(entities, c);
  cout<<"REA"<<endl;
  for(Entity *e : entities) {
    cout<<e->getType()<<endl;
  }
  for(int i = 0; i < entities.size(); ++i) {
    if(entities[i]->isItem()) {
      int pd = static_cast<Item*>(entities[i])->getEffect().passiveDMG;
      HP -= pd;
      if(HP < 0) HP = 0;
    }
    else {
      int chance = rand()%2;
      if (chance == 1) {
        Effect action = {0, 0, 0, 0};
        Character *enemy = static_cast<Character*>(entities[i]);
        action.hp = -ceil((100) * static_cast<Character*>(entities[i])->getATK() / (100 + DEF)); // 50% CHANCE THIS HAPPENS
        ostringstream damage;
        damage << enemy->getATK();
        if (damage.str() == "0") { entityAction = ""; }
        else { entityAction = entities[i]->getType() + " deals " + damage.str() + " damage to PC"; }
        receiveEffect(action);
      }
    }
  }
}

void Player::receiveEffect(Effect effect) {
  int temp_HP = HP + effect.hp + effect.passiveDMG;
  int temp_money = money + effect.money; 
  int temp_ATK = ATK + effect.atk; int temp_DEF = DEF + effect.def;
  HP = (temp_HP < 0) ? 0 : temp_HP;
  HP = ((HP > MAX_HP) && (MAX_HP != -1)) ? MAX_HP : HP;
  money = (temp_money < 0) ? 0 : temp_money;
  ATK = (temp_ATK < 0) ? 0 : temp_ATK;
  DEF = (temp_DEF < 0) ? 0 : temp_DEF;
}


bool Player::isDead() { return HP <= 0; }
bool Player::isComplete() {return floor->complete(c);}

void Player::printStatus() { 
  cout << "Race: "; 
  Racetype(); 
  cout << " Gold: " << money << "\t \t \t \t \t \t  Floor " << onFloor << endl;
  cout << "HP: " << HP << endl; 
  cout << "Atk: " << ATK << endl; 
  cout << "Def: " << DEF << endl;
  cout << "Action: " << playerAction << "\t" << entityAction << endl;
  playerAction = "";
  entityAction = "";
}

void Player::getRender(vector<char> &v) { 
  floor->getRender(v);
  v[c.y*floor->getH()+c.x] = '$';
}

void Player::tickItems() { 
  for (int i = 0; i < items.size(); i++) { 
    if(items[i]->getDuration() > 0) receiveEffect(items[i]->getEffect());
    items[i]->tick(); 
  }
  }

void Player::setCoords(Coords c) {
  this->c.x = c.x;
  this->c.y = c.y;
  this->c.chamber = c.chamber;
}

void Player::setFloor(Floor *floor, int n) { 
  this->floor = floor;
  this->onFloor = n;
}

void Player::quit() {HP = 0;}

bool Player::trade(string dir) {
  Entity *t = floor->getTarget(c, dir, true);
  if(!t->isItem() && static_cast<Character*>(t)->isTrader()) {
    vector<shared_ptr<Item>> shop = static_cast<Merchant*>(t)->getTrades(); 
    if(shop.empty()) return false;
    cout<<"TRADE"<<endl;
    for(int i = 0; i < shop.size(); i++) {
      cout<<"Item "<<(i+1);
      cout<<":\t"<<shop[i]->getType().substr(2);
      cout<<"\tPrice: "<<shop[i]->getPrice()<<endl;
    }
    cout<<"Current gold: "<<money<<endl;
    while(true) {
      cout<<"Enter item number to buy, 0 to exit:"<<endl;
      int bn;
      while(!cin>>bn) {
        continue;
      }
      if(bn == 0) break;
      else if(bn <= shop.size()) {
        if(shop[bn-1]->getPrice() <= money) {
          items.emplace_back(shop[bn-1]);
          money -= shop[bn-1]->getPrice();
          cout<<"BOUGHT: "<<shop[bn]->getType().substr(2)<<endl;
        } else {
          cout<<"NOT ENOUGH GOLD!"<<endl;
        }
      } else {
        cout<<"INVALID ITEM!"<<endl;
      }
    }
  }
  return true;
}

bool Player::check_floor() {
  return true;
}
int Player::getGold() {
  return money;
}
