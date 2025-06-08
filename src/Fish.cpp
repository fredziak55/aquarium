#include "Fish.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cmath>
#include <cstdlib>

Fish::Fish(const char* modelPath, glm::vec3 startPos, glm::vec3 startDir, float speed) 
    : model(modelPath), position(startPos), direction(glm::normalize(startDir)), speed(speed), rotationAngle(0.0f) {}

void Fish::Update(float deltaTime, const glm::vec3& tankMin, const glm::vec3& tankMax) {
    // Move fish
    position += direction * speed * deltaTime;
    
    // Bounce off boundaries by reflecting direction components when limits are reached
    for (int i = 0; i < 3; i++) {
        if (position[i] < tankMin[i]) {
            position[i] = tankMin[i];
            direction[i] = fabs(direction[i]); // Reflect to positive
        }
        if (position[i] > tankMax[i]) {
            position[i] = tankMax[i];
            direction[i] = -fabs(direction[i]); // Reflect to negative
        }
    }
    
    // Apply a slight random perturbation occasionally for more natural movement
    if ((rand() % 1000) < 5) {
        changeDirection();
    }
    
    // Calculate rotation angle based on horizontal direction (ignoring y component)
    rotationAngle = atan2(direction.z, direction.x);
}

void Fish::Draw(Shader &shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    
    shader.setMat4("model", modelMatrix);
    model.Draw(shader);
}

void Fish::changeDirection() {
    // Generate a small random offset vector (range[-0.25, 0.25])
    glm::vec3 randomOffset(
        ((rand() % 50) - 25) / 100.0f,
        ((rand() % 50) - 25) / 100.0f,
        ((rand() % 50) - 25) / 100.0f
    );
    direction += randomOffset;
    
    // Keep movement mostly horizontal
    direction.y *= 0.3f;
    direction = glm::normalize(direction);
}