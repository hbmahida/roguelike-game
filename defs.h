#ifndef DEFS
#define DEFS
#include <cstdlib> //convenience include here because pretty much everything that uses these definitions also does RNG
//definitions of structs used elsewhere
struct Coords {
    int x, y;
    int chamber; //set to -1 if it doesn't matter (i.e. for stairs)
    //this is mainly used for optimization
    
    bool operator==(const Coords &rhs) ;
    bool operator!=(const Coords &rhs) ;
    static bool c_compare(Coords c1, Coords c2);
};
struct Effect {
    int hp = 0;
    int atk = 0;
    int def = 0;
    int money = 0;
    int passiveDMG = 0;
    Effect &operator+=(Effect &other) ;
};

#endif
