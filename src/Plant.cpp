#include "Plant.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Plant::Plant(const char* modelPath, glm::vec3 pos) 
    : model(modelPath), position(pos), swayFactor((rand() % 100) / 100.0f), swayTime(0.0f) {}

void Plant::Update(float deltaTime) {
    swayTime += deltaTime;
}

void Plant::Draw(Shader &shader) {
    // Calculate sway animation
    float sway = sin(swayTime * 2.0f + position.x * 5.0f + position.z * 5.0f) * 0.1f * swayFactor;
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, sway, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
    
    shader.setMat4("model", modelMatrix);
    model.Draw(shader);
}