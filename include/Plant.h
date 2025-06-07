#ifndef PLANT_H
#define PLANT_H

#include "Model.h"
#include <glm/glm.hpp>

class Plant {
public:
    Model model;
    glm::vec3 position;
    float swayFactor;
    float swayTime;
    
    Plant(const char* modelPath, glm::vec3 pos);
    void Update(float deltaTime);
    void Draw(Shader &shader);
};

#endif