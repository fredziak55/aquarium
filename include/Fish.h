#ifndef FISH_H
#define FISH_H

#include "Model.h"
#include <glm/glm.hpp>

class Fish {
public:
    Model model;
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float rotationAngle;
    
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetDirection() const { return direction; }
    void SetDirection(const glm::vec3& newDir) { direction = glm::normalize(newDir); }

    Fish(const char* modelPath, glm::vec3 startPos, glm::vec3 startDir, float speed);
    void Update(float deltaTime, const glm::vec3& tankMin, const glm::vec3& tankMax);
    void Draw(Shader &shader);
    
private:
    void changeDirection();
};

#endif