#include "hero.h"
#include "player.h"
#include "defs.h"

// Shade
Shade::Shade() : Player{125, 25, 25} { setMAX_HP(125); }

void Shade::Racetype() { cout << "Shade"; }


// Drow
Drow::Drow() : Player{150, 25, 15} { setMAX_HP(150); }

void Drow::Racetype() { cout << "Drow"; }

void Drow::receiveEntityActions() {
  vector<Entity*> entities;
  floor->inRange(entities, c);
  for(int i = 0; i < entities.size(); ++i) {
    if(entities[i]->isItem()) {
      Effect action = static_cast<Item*>(entities[i])->getEffect();
      action.hp *= 1.5;
      action.atk *= 1.5;
      action.def *= 1.5;
      action.passiveDMG *= 1.5;
      receiveEffect(action);
    }
    else {
      int chance = rand()%2;
      if (chance == 1) {
        Effect action = {0, 0, 0, 0};
        action.hp = -ceil((100) * static_cast<Character*>(entities[i])->getATK() / (100 + DEF)); // 50% CHANCE THIS HAPPENS
        ostringstream damage;
        damage << static_cast<Character*>(entities[i])->getATK();
        if (damage.str() == "0") { entityAction = ""; }
        else { entityAction = entities[i]->getType() + " deals " + damage.str() + " damage to PC"; }
        receiveEffect(action);
      }
    }
  }
}


// Vampire
Vampire::Vampire() : Player{50, 25, 25} { setMAX_HP(-1); }

void Vampire::Racetype() { cout << "Vampire"; }

bool Vampire::attack(string dir) {
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
    if (entity->getType() == "D Dwarf") { HP = HP < 5 ? 0 : HP - 5; }
    else { HP = old_HP == enemy->getHP() ? HP : HP + 5; }
    return true; 
  }
  return false;
}


// Troll
Troll::Troll() : Player{120, 25, 15} { setMAX_HP(120); }

void Troll::Racetype() { cout << "Troll"; }

void Troll::tickItems() { 
  for (int i = 0; i < items.size(); i++) { if(items[i]->getDuration() > 0) receiveEffect(items[i]->getEffect());
    items[i]->tick(); }
  if(MAX_HP > HP + 5) { HP += 5; }
  else HP = MAX_HP;
}


// Goblin
Goblin::Goblin() : Player{110, 15, 20} { setMAX_HP(110); }

void Goblin::Racetype() { cout << "Goblin"; }

void Goblin::receiveEntityActions() {
  vector<Entity*> entities;
  floor->inRange(entities, c);
  for(int i = 0; i < entities.size(); ++i) {
    if(entities[i]->isItem()) {
      Effect action = static_cast<Item*>(entities[i])->getEffect();
      receiveEffect(action);
    }
    else {
      int chance = rand()%2;
      if (chance == 1) {
        Effect action = {0, 0, 0, 0};
        Character *enemy = static_cast<Character*>(entities[i]);
        ostringstream damage;
        damage << enemy->getATK();
        if (damage.str() == "0") { entityAction = ""; }
        else { entityAction = entities[i]->getType() + " deals " + damage.str() + " damage to PC"; }
        if (entities[i]->getType() == "O Orc") {
          action.hp = -1.5 * ceil((100) * static_cast<Character*>(entities[i])->getATK() / (100 + DEF));
        }
        else { action.hp = -ceil((100) * static_cast<Character*>(entities[i])->getATK() / (100 + DEF)); } // 50% CHANCE THIS HAPPENS
        receiveEffect(action);
      }
    }
  }
}

bool Goblin::attack(string dir) {
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
      money = enemy->getHP() <= 0 ? money + 5 : money;
      return true; 
  }
  return false;
}
