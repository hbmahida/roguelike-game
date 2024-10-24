#include "player.h"
#include "gamemap.h"
#include "entity.h"
#include "hero.h"
#include "defs.h"
#include <string>
#include <iostream>
#include <time.h>
#define ESC "\033["
#define RESET "\033[m"

using namespace std;

void render(vector<char> &v, int ho, int ve) {
    cout<<v.size()<<endl;
    for(int i = 0; i < ve; i++) {
        for(int j = 0; j < ho; j++) {
            char cc = v[i*ho+j];
            if(cc == '$') { //players must be blue, so we highlight currently active player with light blue
                cc = '@';
                cout<<ESC<<"96m"<<cc<<RESET;
            } else if(cc == '@') { //other players are dark blue
                cout<<ESC<<"94m"<<cc<<RESET;
            } else if(cc == 'P') { //potions are green
                cout<<ESC<<"92m"<<cc<<RESET;
            } else if(cc == 'G') { //gold is yellow
                cout<<ESC<<"93m"<<cc<<RESET;
            } else if(cc == 'M') { //merchants are purple (just to differentiate them)
                cout<<ESC<<"95m"<<cc<<RESET;
            } else if('A' <= cc && cc <= 'Z') { //enemies are red otherwise
                cout<<ESC<<"91m"<<cc<<RESET;
            } else {
                cout<<cc;
            }
        }
        cout<<endl;
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    bool dlc = false; //gots through the default spawn files
    bool test = false; //allows you to specify the spawn file
    for(int i = 0; i <= argc; i++) {
        if(argv[i] == "-dlc") dlc = true;
        if(argv[i] == "-test") test = true;
    }
    vector<string> levels;
    //add levels here
    levels.push_back("level1.cc3f");
    levels.push_back("level2.cc3f");
    levels.push_back("level3.cc3f");
    levels.push_back("level4.cc3f");
    levels.push_back("level5.cc3f");
    gamestart:
    //players
    bool complete = false; //will be true at the end if all things completed successfully
    bool dead = false;     //will be true at the end if everyone dies 
    vector<shared_ptr<Player>> players;
        int pn = 1;
        if(dlc) {
            cout<<"Enter number of players: ";
            cin>>pn;
            pn = max(pn, 4);
            pn = min(pn, 1);
        }
        for(int i = 0; i < pn; i++) {
            cout<<"Enter character race: ";
            string type;
            cin>>type;
            if(type == "d") {
                players.push_back(make_shared<Drow>());
                cout<<"DROW selected!"<<endl;
            } else if(type == "v") {
                players.push_back(make_shared<Vampire>());
                cout<<"VAMPIRE selected!"<<endl;
            } else if(type == "t") {
                players.push_back(make_shared<Troll>());
                cout<<"TROLL selected!"<<endl;
            } else if(type == "g") {
                players.push_back(make_shared<Goblin>());
                cout<<"GOBLIN selected!"<<endl;
            //} else if(type == "healer" && dlc) {
            //    players.push_back(make_shared<Healer>(new Healer()));
            //    cout<<"HEALER selected!"<<endl;
            } else {
                players.push_back(make_shared<Shade>());
                cout<<"SHADE selected!"<<endl;
            }
        }
    for(string lev : levels) {
        complete = false;
        dead = false;
        string clev = lev;
        if(test) { //run 1 level as a test
            cout<<"Enter the level filename: ";
            string fname;
            cin>>fname;
            clev = fname;
        }
        shared_ptr<Floor> cur = make_shared<Floor>(clev, !test, dlc);
        //drop players in
        for(int i = 0; i < players.size(); i++) {
            if(!players[i]->isDead()) {
                players[i]->setFloor(cur.get(), i+1);
                players[i]->check_floor();
                players[i]->setCoords(cur->addPlayer());
            }
        }

        //main game loop
        while(!complete && !dead) {
            complete = true;
            dead = true;
            for(int i = 0; i < players.size(); i++) {
                if(!players[i]->isComplete() && !players[i]->isDead()) {
                    complete = false;
                    dead = false;
                    vector<char> rend;
                    players[i]->getRender(rend);
                    render(rend, cur->getH(), cur->getV());
                    players[i]->printStatus();
                    string command;
                    cin>>command;
                    while(true) {
                        if(command.length() == 2) {
                            players[i]->move(command);
                            players[i]->tickItems();
                            players[i]->receiveEntityActions();
                            break;
                        } else if(command == "a") {
                            string dir;
                            cin>>dir;
                            players[i]->attack(dir);
                            players[i]->tickItems();
                            players[i]->receiveEntityActions();
                            break;
                        } else if(command == "t") {
                            string dir;
                            cin>>dir;
                            players[i]->trade(dir);
                            players[i]->tickItems();
                            players[i]->receiveEntityActions();
                            break;
                        } else if(command == "r") {
                            goto gamestart; //I have committed a sin, but this command is a certified bruh moment
                        } else if(command == "q") {
                            players[i]->quit(); //the player just straight up dies lmao
                            break;
                        }
                        cin>>command;
                    }
                } else if(!players[i]->isDead()) {
                    complete = true;

                } else {
                    dead = true;
                }
            }
            cur->updateEntities();
        }
        if(test) break; //test plays 1 level at a time
    }
    cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nGAME OVER"<<endl;
    for(int i = 0; i < players.size(); i++) {
        if(players[i]) cout<<"Player "<<(i+1)<<" gold gained: "<<players[i]->getGold()<<endl;
    }
}
