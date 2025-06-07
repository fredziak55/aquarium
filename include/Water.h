#ifndef WATER_H
#define WATER_H

#include <vector>
#include "Shader.h"
#include "Texture.h"

class Water {
public:
    Water(float size);
    void Draw(Shader &shader, float time);
    
private:
    unsigned int VAO, VBO;
    Texture texture;
    std::vector<float> vertices;
    void setupWater();
};

#endif