#ifndef _MASS_H_
#define _MASS_H_


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// reference:
// nehe physics simulation tutorial 
// http://nehe.gamedev.net/tutorial/introduction_to_physical_simulations/18005/
class Mass{

public:
    Mass(float m)                               // Constructor.
    {
        this->m = m;
        this->vel = glm::vec3(0);
        this->force = glm::vec3(0);
    }    
    
    void applyForce(glm::vec3 force)
    {
        this->force += force;               // The External Force Is Added To The Force On The Mass.
    }
    
    void init()                             // This Method Sets The Force Values To Zero.
    {
        this->force = glm::vec3(0);
        this->vel = glm::vec3(0);
    }    
    void simulate(float dt)
    {
        vel += (force / m) * dt;           
        pos += vel * dt;                   
    }
    void simulate(float dt);  
    // some function for collision detection  
    virtual ~Mass(){
        
    } 

  
private:
    // collison information e.g. mass size
    float m;                                     // The Mass Value.
    glm::vec3 pos;                               // Position In Space.
    glm::vec3 vel;                               // Velocity.
    glm::vec3 force;                             // Force Applied On This Mass At An Instance.

};


#endif