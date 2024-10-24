#include "defs.h"
bool Coords::operator==(const Coords &rhs) {
		return x == rhs.x && y == rhs.y; //if you're in the same place, you are certainly in the same chamber
}
bool Coords::operator!=(const Coords &rhs) {
		return x != rhs.x || y != rhs.y;
}

bool Coords::c_compare(Coords c1, Coords c2) {
    if(c1.x > c2.x) return true;
    else if(c1.x == c2.x) return c1.y > c2.y;
    else return false;
}
Effect &Effect::operator+=(Effect &other) { hp += other.hp; atk += other.atk; 
                                        def += other.def; money += other.money; passiveDMG += other.passiveDMG; 
                                        return *this;}

