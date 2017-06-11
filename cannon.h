#ifndef CANNON_H
#define CANNON_H
#include <ctgmath> 
struct CannonGroup
{
    vector<GameBodyBase *> cannons;
    vector<float> timer;
    vector<float> frequency;
    void orientPlayer(glm::vec3 player_position)
    {
        for (int i = 0; i < cannons.size(); i++)
        {
            auto dis = player_position - cannons[i]->position;
            auto disx = dis.x;
            auto disz = dis.z;
            // update cannon yaw
            if (disz >= 0) {
                cannons[i]->yaw = -180.0f / 3.1415926f * acos(disx / sqrt(disx * disx+ disz * disz));
            } else {
                cannons[i]->yaw = 180.0f / 3.1415926f * acos(disx / sqrt(disx * disx+ disz * disz));
            }
            cannons[i]->updateBaseVectorsAccordingToSelfAngles();
            // std::cout << player_position.x-cannons[i]->position.x << " " << player_position.z-cannons[i]->position.x << " "<<std::endl;
            // std::cout << cannons[i]->front.x << " " << cannons[i]->front.z << " "<<std::endl;

        }
    }
};

#endif