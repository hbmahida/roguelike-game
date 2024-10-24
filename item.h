#ifndef ITEM_H
#define ITEM_H
#include "entity.h"

class Item: public Entity {
    string type;
    Effect effect;
    bool isLocked;
    int duration;
    int price;

    public:
        Item(Coords c, string type, Effect e, bool lock = false, int dur = 1, int price = 0); // Default constructor
        ~Item() = default;
        Effect getEffect() const; // Returns the Effect struct based on the type of potion used.
        int getDuration() const; // Returns the duration for which the effect is active.
        void tick(); //reduces duration by 1
        bool isItem() override; // Returns true if entity type is item.
        string getType() const override;
        int getPrice() const; // Returns the price of potions for merchant to trade. (DLC Content)

        // For Dragon and dragon horde,
        bool locked(); // Returns true if the item type is dragon horde and dragon is alive.
        void unlock(); // Unlocks the treasure once dragon is slayed.
};

#endif
