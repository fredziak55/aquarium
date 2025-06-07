#include "Water.h"
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

Water::Water(float size) : texture("resources/textures/water.png", "texture_diffuse") {
    // Create a flat plane for water
    float halfSize = size / 2.0f;
    
    vertices = {
        // Positions          // Texture Coords
        -halfSize, 0.0f, -halfSize,  0.0f, 1.0f,
        -halfSize, 0.0f,  halfSize,  0.0f, 0.0f,
         halfSize, 0.0f, -halfSize,  1.0f, 1.0f,
         halfSize, 0.0f,  halfSize,  1.0f, 0.0f,
    };
    
    setupWater();
}

void Water::Draw(Shader &shader, float time) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    shader.setMat4("model", modelMatrix);
    
    texture.bind(0);
    shader.setInt("texture_diffuse1", 0);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Water::setupWater() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
}