#include "gamemap.h"
#include <algorithm>

using namespace std;

Chamber::Chamber(vector<bool> l, int hs, int vs): entities{} {
    for(bool b : l) {
        limits.emplace_back(b);
    }
    hsize = hs;
    vsize = vs;
}
Coords Chamber::getMove(Coords c, string dir, vector<Coords> &ppos) {
    Coords nc = c;
    if(dir == "no") nc.y--;
    else if(dir == "so") nc.y++;
    else if(dir == "ea") nc.x++;
    else if(dir == "we") nc.x--;
    else if(dir == "ne") {nc.y--; nc.x++;}
    else if(dir == "nw") {nc.y--; nc.x--;}
    else if(dir == "se") {nc.y++; nc.x++;}
    else {nc.y++; nc.x--;}
    for(Coords co : ppos) {
        //cout<<co.x<<" "<<co.y<<" checked"<<endl;
        if(nc == co) return c;
    }
    if(getTarget(c, dir, false) == nullptr && getTarget(c, dir, true) == nullptr && limits[nc.y*hsize+nc.x]) { 
        for(int i = 0; i < ppos.size(); i++) {
            if(ppos[i] == c) { 
                ppos[i] = nc;
                break;
            }
        }
        return nc;
    }
    return c;
}
void Chamber::inRange(vector<Entity*> &v, Coords c) {
    v.clear();
    //enemies in range
    if(getTarget(c, "no", true) != nullptr) v.emplace_back(getTarget(c, "no", true));
    if(getTarget(c, "so", true) != nullptr) v.emplace_back(getTarget(c, "so", true));
    if(getTarget(c, "ea", true) != nullptr) v.emplace_back(getTarget(c, "ea", true));
    if(getTarget(c, "we", true) != nullptr) v.emplace_back(getTarget(c, "we", true));
    if(getTarget(c, "ne", true) != nullptr) v.emplace_back(getTarget(c, "ne", true));
    if(getTarget(c, "nw", true) != nullptr) v.emplace_back(getTarget(c, "nw", true));
    if(getTarget(c, "se", true) != nullptr) v.emplace_back(getTarget(c, "se", true));
    if(getTarget(c, "sw", true) != nullptr) v.emplace_back(getTarget(c, "sw", true));
    //get items in range (for passive dmg effect, such as spitball's 1 block radius damage)
    if(getTarget(c, "no", false) != nullptr) v.emplace_back(getTarget(c, "no", false));
    if(getTarget(c, "so", false) != nullptr) v.emplace_back(getTarget(c, "so", false));
    if(getTarget(c, "ea", false) != nullptr) v.emplace_back(getTarget(c, "ea", false));
    if(getTarget(c, "we", false) != nullptr) v.emplace_back(getTarget(c, "we", false));
    if(getTarget(c, "ne", false) != nullptr) v.emplace_back(getTarget(c, "ne", false));
    if(getTarget(c, "nw", false) != nullptr) v.emplace_back(getTarget(c, "nw", false));
    if(getTarget(c, "se", false) != nullptr) v.emplace_back(getTarget(c, "se", false));
    if(getTarget(c, "sw", false) != nullptr) v.emplace_back(getTarget(c, "sw", false));

    //this code is kind of shoehorned in because the dragon is the only entity with this kind of logic
    //essentially, it adds the dragon if you are near a hoard
    bool nearDragon = false;
    for(Entity *e : v) {
        if(e->getType() == "G Dragon Hoard") nearDragon = true;
    }
    if(nearDragon) for(shared_ptr<Entity> &e : entities) {
        if(e->getType() == "D Dragon") v.emplace_back(e.get());
    }

    //for(Entity *e : v) cout<<e->getType()<<" in range"<<endl;
}
Entity *Chamber::getTarget(Coords c, string dir, bool isAttack) {
    //get new coords
    Coords nc = c;
    if(dir == "no") nc.y--;
    else if(dir == "so") nc.y++;
    else if(dir == "ea") nc.x++;
    else if(dir == "we") nc.x--;
    else if(dir == "ne") {nc.y--; nc.x++;}
    else if(dir == "nw") {nc.y--; nc.x--;}
    else if(dir == "se") {nc.y++; nc.x++;}
    else {nc.y++; nc.x--;}
    for(int i = 0; i < entities.size(); i++) {
        if(entities[i]->getLocation() == nc) {
            if((isAttack && entities[i]->isItem() == false) || (!isAttack && entities[i]->isItem() == true && !static_cast<Item*>(entities[i].get())->locked())) {
                return entities[i].get();
            } 
        }
    }
    return nullptr;
}
bool compare(shared_ptr<Entity>& e1, shared_ptr<Entity>& e2) { //helper for updateEntities to do entity updates in the correct order
    return Coords::c_compare(e1->getLocation(), e2->getLocation());
}
void Chamber::updateEntities(vector<Coords> &ppos) {
    vector<shared_ptr<Entity>> addback;
    //remove dead things and get their drops/unlocks
    while(!entities.empty()) {
        if(entities.back()->isItem()) {
            addback.emplace_back(entities.back());
            entities.pop_back();
        } else if(static_cast<Character*>(entities.back().get())->getHP() == 0) {
            shared_ptr<Entity> drop = static_cast<Character*>(entities.back().get())->getDrop();
            if(drop) addback.emplace_back(drop);
            entities.pop_back();
        } else {
            addback.emplace_back(entities.back());
            entities.pop_back();
        }
    }
    while(!addback.empty()) {
        spawnEntity(move(addback.back()), ppos);
        addback.pop_back();
    }
    for(shared_ptr<Entity> &e : entities) {
        if(!e->isItem() && static_cast<Character*>(e.get())->isSpawner()) {
            Spawner *temp = static_cast<Spawner*>(e.get());
            int rfreq = temp->getFreq();
            //decision on if the spawner will spawn this turn
            bool willSpawn = false;
            if(rfreq > 0) { //tickcount spawning
                willSpawn = (updates%rfreq == 0);
            }else if(rfreq < 0) { //RNG spawning
                willSpawn = (rand()%(-rfreq) == 0);
            }
            if(willSpawn) {
                shared_ptr<Entity> nchild = move(temp->getNewChild());
                if(nchild->getLocation().chamber != temp->getLocation().chamber) { //random spawning (invalid chamber)
                    //note that chamber is not further set because it is not needed once the child is in the chamber
                    addEntity(move(nchild), ppos);
                } else {
                    spawnEntity(move(nchild), ppos);
                }
            }
        }
    }
    //sort and move
    sort(entities.begin(), entities.end(), compare); //sort using the above comparator
    for(int i = 0; i < entities.size(); i++) {
        if(!entities[i]->isItem()) {
            string dirs[8] = {"no", "so", "ea", "we", "ne", "nw", "se", "sw"};
            int ctr = 100; //try 10 times to move (very unlikely that it will fail)
            while(ctr > 0) {
                string rdir = dirs[rand()%8];
                if(entities[i]->getLocation() != getMove(entities[i]->getLocation(), rdir, ppos)) {
                    static_cast<Character*>(entities[i].get())->move(rdir);
                    //cout<<"moved entity"<<i<<" in direction "<<rdir<<endl;
                    break;
                }
                ctr--;
            }
        }
    }
    updates++;
}
bool Chamber::addEntity(shared_ptr<Entity> e, vector<Coords> &ppos) {
    vector<bool> curav = limits;
    for(shared_ptr<Entity> &en : entities) {
        curav[en->getLocation().y*hsize+en->getLocation().x] = false;
    }
    for(Coords c : ppos) { //player positions
        curav[c.y*hsize+c.x] = false;
    }
    bool hasSpace = false;
    for(bool b : curav) {
        if(b) hasSpace = true;
    }
    if(!hasSpace) return false;
    while(true) {
        int cc = rand()%curav.size();
        if(curav[cc]) {
            e->setLocation(cc%hsize, cc/hsize);
            break;
        }
    }
    entities.emplace_back(move(e));
    if(entities.back()->getType() == "G Dragon Hoard") { //spawn a dragon (this code is "bolted on" because it's pretty much the only time this logic is used)
        if(!addEntity(make_shared<Dragon>(Coords{0, 0, entities.back()->getLocation().chamber}, static_cast<Item*>(entities.back().get())), ppos)) {
            entities.pop_back();
            return false;
        }
    }
    return true;
}
bool Chamber::spawnEntity(shared_ptr<Entity> e, vector<Coords> &ppos) { //assume e has already been set correctly, but check for conflict (in which case return false)
    if(!limits[e->getLocation().y*hsize+e->getLocation().x]) return false;
    for(shared_ptr<Entity> &en : entities) {
        if(en->getLocation() == e->getLocation()) return false;
    }
    for(Coords c : ppos) {
        if(e->getLocation() == c) return false;
    }
    entities.emplace_back(move(e));
    return true;
}
void Chamber::getAllEntities(vector<Entity*> &v) {
    v.clear();
    for(int i = 0; i < entities.size(); i++) {
        v.emplace_back(entities[i].get());
    }
}
shared_ptr<Item> Chamber::pickup(Entity *i) { //assumes we already have a valid Entity pointer (since it would only be called AFTER getTarget() is called)
    int idx;
    for(idx = 0; idx < entities.size(); idx++) {
        if(entities[idx].get() == i) break;
    }
    shared_ptr<Item> temp = shared_ptr<Item>(static_cast<Item*>(entities[idx].get())); //this has to be done before erasing it or it will be dealloced
    entities.erase(entities.begin()+idx);
    return temp;
}

Floor::Floor(string file, bool plain, bool dlc)  {
    //does the merchant sell stuff?
    vector<shared_ptr<Item>> shop;
    if(dlc) {
        shop.emplace_back(make_shared<Item>(Coords{0, 0, -1}, "P Restore Health", Effect{10, 0, 0, 0, 0}, false, 1, 20));
        shop.emplace_back(make_shared<Item>(Coords{0, 0, -1}, "P Increase ATK", Effect{10, 0, 0, 0, 0}, false, 1, 15));
        shop.emplace_back(make_shared<Item>(Coords{0, 0, -1}, "P Increase DEF", Effect{10, 0, 0, 0, 0}, false, 1, 10));
        shop.emplace_back(make_shared<Item>(Coords{0, 0, -1}, "P Passive Heal", Effect{2, 0, 0, 0, 0}, false, 50, 25)); //passive heal for 2 health per game tick, duration 50 ticks
    }
    ifstream lin;
    lin.open(file);
    lin>>hsize>>vsize; //size constraints
    string st; //clearing getline out
    getline(lin, st);
    for(int i = 0; i < vsize; i++) { //reads the raw map (if plain is false, this is the map used)
        string s;
        getline(lin, s);
        for(int j = 0; j < hsize; j++) {
            map.emplace_back(s[j]);
        }
    }
    //stack flood fill algorithm for room creation (pretty much BFS but with stacks and not queues)
    //BFS is not taught in CS246 but I think we all used it in CCC at some point
    //this is pretty slow, but only runs once at the level start so it isn't that bad (and is nostalgic)
    vector<int> bmap;
    vector<int> sx;
    vector<int> sy;
    for(int i = 0; i < vsize; i++) { //reads the raw map (if plain is false, this is the map used)
        for(int j = 0; j < hsize; j++) {
            if(map[i*hsize+j] != '|' && map[i*hsize+j] != ' ' && map[i*hsize+j] != '-' && map[i*hsize+j] != '#') {
                bmap.emplace_back(i*hsize+j); //we know [0, 0] is invalid (since it's the corner of the boundary), so 0 is never an option
                sx.emplace_back(j);
                sy.emplace_back(i);
            } else bmap.emplace_back(0);
        }
    }
    while(!sx.empty()) { //the stacks are synchronized because they are just the xy coords which are popped at the same time
        int cx = sx.back();
        sx.pop_back();
        int cy = sy.back();
        sy.pop_back();
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                if((i+j == 1 || i+j == -1) && cx+j > 0 && cy+i > 0 && cx+j < hsize && cy+i < vsize && bmap[(cy+i)*hsize+(cx+j)] != 0 && bmap[(cy+i)*hsize+(cx+j)] > bmap[cy*hsize+cx]) {
                    bmap[(cy+i)*hsize+(cx+j)] = bmap[cy*hsize+cx];
                    sx.emplace_back(cx+j);
                    sy.emplace_back(cy+i);
                }
            }
        }
    }
    //get difficulty here for chamber creating later
    //we will give "custom" floors a difficulty of 0 to ensure completely manual spawning
    int difficulty;
    lin>>difficulty; //default value should be 20
    if(!dlc) difficulty = 20;
    //get number of potions
    int potions;
    lin>>potions; //default is 10
    if(!dlc) potions = 10;
    //get treasure number
    int treasure;
    lin>>treasure;
    if(!dlc) treasure = 10;
    //numbering off chambers and populating them (also slow, but runs once)
    vector<int> cnum;
    for(int i = 0; i < bmap.size(); i++) { //this truncates the numbering in bmap (which may be non-continuous)
        if(bmap[i] != 0 && (cnum.empty() || bmap[i] > cnum.back())) cnum.emplace_back(bmap[i]);
    }
    for(int i = 0; i < cnum.size(); i++) {
        vector<bool> l;
        for(int j = 0; j < bmap.size(); j++) {
            if(bmap[j] == cnum[i]) { 
                l.emplace_back(true);
            } else l.emplace_back(false);
        }
        chambers.emplace_back(l, hsize, vsize);
    }
    for(int i = 0; i < bmap.size(); i++) {
        for(int j = 0; j < cnum.size(); j++) {
            if(bmap[i] == 0) bmap[i] = -1; //invalid cell for movement
            if(bmap[i] == cnum[j]) bmap[i] = j; //truncates bmap BEFORE spawn logic, since it is needed to put the entities into the right rooms
        }
    }
    //there is a bit of a change, now entity spawning is handled by floor (since there is a cap of <difficulty> per floor, default difficulty being 20)
    if(plain) { //RNG logic (everything but player, which is handled by the player class)
        //stairs
        for(int i = 0; i < 1000; i++) { //just in case someone creates a map with 0 space
            int rx = rand()%hsize;
            int ry = rand()%vsize;
            if(map[ry*hsize+rx] == '.') {
                stairs.x = rx;
                stairs.y = ry;
                stairs.chamber = bmap[ry*hsize+rx];
                break;
            }
        }
        //potions (everything will have abbreviation P because the effects are hidden until use)
        for(int i = 0; i < potions; i++) {
            int ctr = 1000; //used so it doesn't loop forever but will most probably get all the spawns
            while(ctr > 0) {
                string type = "";
                Effect e = {0, 0, 0, 0, 0};
                switch(rand()%6) {
                    case 0: type = "P Restore health";
                            e.hp = 10;
                    break;
                    case 1: type = "P Boost ATK";
                            e.atk = 5;
                    break;
                    case 2: type = "P Boost DEF";
                            e.def = 5;
                    break;
                    case 3: type = "P Poison health";
                            e.hp = -10;
                    break;
                    case 4: type = "P Wound ATK";
                            e.atk = -5;
                    break;
                    default: type = "P Wound DEF";
                            e.def = -5;
                }
                int ac = rand()%chambers.size();
                if(addEntity(make_shared<Item>(Coords{0, 0, ac}, type, e, false), ac)) break;
            }
        }
        //gold
        for(int i = 0; i < treasure; i++) {
            int ctr = 1000; //used so it doesn't loop forever but will most probably get all the spawns
            while(ctr > 0) {
                string type = "";
                Effect e = {0, 0, 0, 0, 0};
                bool locked = false;
                int rt = rand()%8;
                if(rt <= 4) {
                    type = "G Small Gold";
                    e.money = 1;
                } else if(rt <= 5) {
                    type = "G Dragon Hoard";
                    e.money = 6;
                    locked = true;
                } else {
                    type = "G Normal Gold";
                    e.money = 2;
                }
                int ac = rand()%chambers.size();
                if(addEntity(make_shared<Item>(Coords{0, 0, ac}, type, e, locked), ac)) break; //dragon spawning will happen in the addEntity of the chamber (since it will be linked to the gold)
                ctr--;
            }
        }
        //enemy spawning
        for(int i = 0; i < difficulty; i++) {
            int ctr = 1000; //used so it doesn't loop forever but will most probably get all the spawns
            while(ctr > 0) {
                int rt = rand()%18;
                int ac = rand()%chambers.size();
                if(rt < 4) {
                    if(chambers[ac].addEntity(make_shared<Human>(Coords{0, 0, ac}), ppos)) break;
                } else if(rt < 7) {
                    if(chambers[ac].addEntity(make_shared<Dwarf>(Coords{0, 0, ac}), ppos)) break;
                } else if(rt < 12) {
                    if(chambers[ac].addEntity(make_shared<Halfling>(Coords{0, 0, ac}), ppos)) break;
                } else if(rt < 14) {
                    if(chambers[ac].addEntity(make_shared<Elf>(Coords{0, 0, ac}), ppos)) break;
                } else if(rt < 16) {
                    if(chambers[ac].addEntity(make_shared<Orc>(Coords{0, 0, ac}), ppos)) break;
                } else {
                    if(chambers[ac].addEntity(make_shared<Merchant>(Coords{0, 0, ac}, shop), ppos)) break;
                }
                ctr--;
            }
        }
        if(dlc) {
            //DLC entity spawning
            int mdn = difficulty/10; //for the default difficulty, spawns 2 mandrakes
            int vn = difficulty/10; //2 rooms with vine spawner default
            int sn = difficulty/6; //2 spitters a map default
            for(int i = 0; i < mdn; i++) {
                int ctr = 1000; //used so it doesn't loop forever but will most probably get all the spawns
                while(ctr > 0) {
                    int ac = rand()%chambers.size();
                    if(chambers[ac].addEntity(make_shared<Mandrake>(Coords{0, 0, ac}), ppos)) break;
                    ctr--;
                }
            }
            for(int i = 0; i < vn; i++) { //spawn a vine root at the top left corner of chosen chambers
                int ac = rand()%chambers.size();
                int lookfor = cnum[ac];
                int idx = 0;
                for(int j = 0; j < map.size(); j++) {
                    if(bmap[j] == lookfor) {
                        idx = j;
                        break;
                    }
                }
                idx -= (hsize+1);
                int cx = idx%hsize;
                int cy = idx/hsize;
                chambers[ac].spawnEntity(make_shared<VineHead>(Coords{cx, cy, ac}), ppos);
            }
            for(int i = 0; i < sn; i++) {
                int ctr = 1000; //used so it doesn't loop forever but will most probably get all the spawns
                while(ctr > 0) {
                    int ac = rand()%chambers.size();
                    if(chambers[ac].addEntity(make_shared<Spitter>(Coords{0, 0, ac}), ppos)) break;
                    ctr--;
                }
            }
        }
    } else { //predetermined spawns
        //manual items
        vector<shared_ptr<Item>> mp;
        vector<shared_ptr<Item>> mg;
        vector<shared_ptr<Dragon>> md;
        for(int i = 0; i < potions; i++) {
            string pn;
            getline(lin, pn);
            int px, py, pc;
            cin>>px>>py>>pc;
            int hv, av, dv, dur;
            cin>>hv>>av>>dv>>dur;
            mp.emplace_back(make_shared<Item>(Coords{px, py, pc}, pn, Effect{hv, av, dv, 0, 0}, false, dur));
        }

        for(int i = 0; i < treasure; i++) {
            string pn;
            getline(lin, pn);
            int tx, ty, tc;
            int val;
            cin>>val;
            mg.emplace_back(make_shared<Item>(Coords{tx, ty, tc}, pn, Effect{0, 0, 0, val, 0}, true));
            if(val == 6) { //which dragon the hoard gets tracked by
                int dx, dy, dc;
                cin>>dx>>dy>>dc;
                md.emplace_back(make_shared<Dragon>(Coords{dx, dy, dc}, mg.back().get()));
            }
        }

        int pc = 0; //potions read
        int gc = 0; //hoards read
        int dc = 0; //dragons read
        for(int i = 0; i < map.size(); i++) {
            int cx = i%hsize; //row-by-row organization like in the question from A2
            int cy = i/hsize;
            //due to concerns about breaking game logic (mostly spawn mechanics), child instances are NOT read in here
            switch(map[i]) {
                case 'H': 
                    chambers[bmap[i]].spawnEntity(make_shared<Human>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'W': 
                    chambers[bmap[i]].spawnEntity(make_shared<Dwarf>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'E': 
                    chambers[bmap[i]].spawnEntity(make_shared<Elf>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'O': 
                    chambers[bmap[i]].spawnEntity(make_shared<Orc>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'M': 
                    chambers[bmap[i]].spawnEntity(make_shared<Merchant>(Coords{cx, cy, bmap[i]}, shop), ppos);
                    map[i] = '.';
                break;
                case 'D': 
                    chambers[bmap[i]].spawnEntity(move(md[dc]), ppos);
                    dc++;
                    map[i] = '.';
                break;
                case 'L': 
                    chambers[bmap[i]].spawnEntity(make_shared<Halfling>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'S': 
                    chambers[bmap[i]].spawnEntity(make_shared<Spitter>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'P': 
                    chambers[bmap[i]].spawnEntity(move(mp[pc]), ppos);
                    pc++;
                    map[i] = '.';
                break;
                case 'G': 
                    chambers[bmap[i]].spawnEntity(move(mg[gc]), ppos);
                    gc++;
                    map[i] = '.';
                break;
                case 'A': 
                    chambers[bmap[i]].spawnEntity(make_shared<Mandrake>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'R':
                    map[i] = '.';
                break;
                case 'I': 
                    chambers[bmap[i]].spawnEntity(make_shared<VineHead>(Coords{cx, cy, bmap[i]}), ppos);
                    map[i] = '.';
                break;
                case 'V': 
                    map[i] = '.';
                break;
                case '/':
                    stairs = {cx, cy, bmap[i]};
                    map[i] = '.';
                break;
            }
        }
    }
    for(int i = 0; i < bmap.size(); i++) {
        roomMap.emplace_back(bmap[i]);
    }
    cs = rand()%chambers.size();
    while(cs == stairs.chamber) cs = rand()%chambers.size(); //DO NOT make everything 1 chamber!
}
Coords Floor::addPlayer() {
    vector<Entity*> ae;
    chambers[cs].getAllEntities(ae);
    vector<int> allvalid;
    for(int i = 0; i < roomMap.size(); i++) {
        if(roomMap[i] == cs) {
            allvalid.emplace_back(i);
        }
    }
    Coords np = {0, 0, cs};
    for(int i = 0; i < 100; i++) {
        int rg = rand()%allvalid.size(); //breaks when entities completely take up the spawn room (very rare)
        bool taken = false;
        for(Entity *en : ae) {
            if(en->getLocation().y*hsize+en->getLocation().x == allvalid[rg]) taken = true;
        }
        for(Coords c : ppos) {
            if(c.y*hsize+c.x == rg) taken = true;
        }
        if(!taken) {
            np.x = allvalid[rg]%hsize;
            np.y = allvalid[rg]/hsize;
        }
    }
    ppos.emplace_back(np);
    return np;
}
Coords Floor::getMove(Coords c, string dir) {
    //get new coords
    Coords nc = c;
    if(dir == "no") nc.y--;
    else if(dir == "so") nc.y++;
    else if(dir == "ea") nc.x++;
    else if(dir == "we") nc.x--;
    else if(dir == "ne") {nc.y--; nc.x++;}
    else if(dir == "nw") {nc.y--; nc.x--;}
    else if(dir == "se") {nc.y++; nc.x++;}
    else {nc.y++; nc.x--;}
    //definitely invalid cases
    if(nc.x < 0 || nc.y < 0 || nc.x >= hsize || nc.y >= vsize || map[nc.y*hsize+nc.x] == '-' || map[nc.y*hsize+nc.x] == '|' || map[nc.y*hsize+nc.x] == ' ') return c;
    nc.chamber = roomMap[nc.y*hsize+nc.x];
    if(getTarget(c, dir, true) == nullptr && getTarget(c, dir, false) == nullptr) {
        for(Coords co : ppos) {
            if(co == nc) return c; //player in the way
        }
        for(int i = 0; i < ppos.size(); i++) {
            if(c == ppos[i]) { //update player-occupied cells
                //cout<<"PPOS: "<<ppos[i].x<<" "<<ppos[i].y<<" NEW: "<<nc.x<<" "<<nc.y<<endl;
                ppos.erase(ppos.begin()+i);
                ppos.push_back(nc);
                //cout<<"PPOS ALL:"<<endl;
                //for(int j = 0; j < ppos.size(); j++) {
                    //cout<<ppos[j].x<<" "<<ppos[j].y<<endl;
                //}
                return nc; //not occupied by anything, move
            }
        }
    }
    return c;
}
void Floor::inRange(vector<Entity*> &v, Coords c) {
    v.clear();
    if(c.chamber < 0 || c.chamber >= chambers.size()) return; //you are not in a chamber
    chambers[c.chamber].inRange(v, c);
}
Entity *Floor::getTarget(Coords c, string dir, bool isAttack) {
    if(c.chamber < 0 || c.chamber >= chambers.size()) return nullptr; //invalid position
    return chambers[c.chamber].getTarget(c, dir, isAttack);
}
void Floor::updateEntities() {
    for(int i = 0; i < chambers.size(); i++) {
        chambers[i].updateEntities(ppos);
    }
}
bool Floor::complete(const Coords &c) {
    int x = c.x;
    int y = c.y;
    int cham = c.chamber;
    int sx = stairs.x;
    int sy = stairs.y;
    int scham = stairs.chamber;
    return sx == x && sy == y && scham == cham;
}
bool Floor::addEntity(shared_ptr<Entity> e, int cham) {
    if(cham < 0 || cham >= chambers.size()) return false;
    return chambers[cham].addEntity(move(e), ppos);
}
bool Floor::spawnEntity(shared_ptr<Entity> e) {
    if(e->getLocation().chamber < 0 || e->getLocation().chamber >= chambers.size()) return false;
    
    return chambers[e->getLocation().chamber].spawnEntity(move(e), ppos);
}
void Floor::getRender(vector<char> &v) {
    v.clear();
    for(int i = 0; i < map.size(); i++) v.push_back(map[i]);
    for(Chamber c : chambers) {
        vector<Entity*> ce;
        c.getAllEntities(ce);
        for(int i = 0; i < ce.size(); i++) {
            v[ce[i]->getLocation().y*hsize+ce[i]->getLocation().x] = ce[i]->getType()[0]; //first character of the type string is the abbreviation
        }
    }
    for(Coords co : ppos) {
        v[co.y*hsize+co.x] = '@'; //pretend you never saw this if you are a TA
    }
    v[stairs.y*hsize+stairs.x] = '/';
}
shared_ptr<Item> Floor::pickup(Entity *i) {
    return chambers[i->getLocation().chamber].pickup(i);
}
