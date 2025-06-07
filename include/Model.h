#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    Model(const char* path);
    void Draw(Shader &shader);
    
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
    void loadModel(const char* path);
};

#endif