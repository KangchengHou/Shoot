#ifndef CANNON_H
#define CANNON_H

struct CannonGroup {
    vector<GameBodyBase*> cannons;
    vector<float> timer;
    vector<float> frequency;
};


#endif