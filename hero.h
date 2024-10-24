#ifndef HERO_H
#define HERO_H
#include <vector>
#include <sstream>
#include "player.h"
#include "defs.h"

class Shade : public Player {
  public:
    Shade();
    void Racetype() override;
};

class Drow : public Player {
  public:
    Drow();
    void Racetype() override;
    void receiveEntityActions() override;
};

class Vampire : public Player {
  public:
    Vampire();
    void Racetype() override;
    bool attack(string dir) override;
};

class Troll : public Player {
  public:
    Troll();
    void Racetype() override;
    void tickItems() override;
};

class Goblin : public Player {
  public:
    Goblin();
    void Racetype() override;
    void receiveEntityActions() override;
    bool attack(string dir) override;
};

#endif
