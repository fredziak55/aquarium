#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture {
public:
    unsigned int ID;
    std::string type;
    std::string path;
    
    Texture(const char* path, const std::string &type);
    void bind(unsigned int unit);
    
private:
    void loadTexture(const char* path);
};

#endif