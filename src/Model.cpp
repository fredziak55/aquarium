#include "Model.h"
#include <glad/glad.h>  // For OpenGL functions
#include <iostream>      // For std::cout
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const char* path) {
    loadModel(path);
    setupMesh();
}

void Model::Draw(Shader &shader) {
    unsigned int diffuseNr = 1;
    
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        
        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }
    glActiveTexture(GL_TEXTURE0);
    
    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    
    glBindVertexArray(0);
}

void Model::loadModel(const char* path) {
    std::cout << "Loading model: " << path << std::endl;
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_FlipUVs);
    
    // Add extensive error checking
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    
    if(scene->mNumMeshes == 0) {
        std::cerr << "ERROR: Model contains no meshes" << std::endl;
        return;
    }
    
    aiMesh* mesh = scene->mMeshes[0];
    if(!mesh) {
        std::cerr << "ERROR: First mesh is null" << std::endl;
        return;
    }

    // Add vertex data with null checks
    if(!mesh->mVertices) {
        std::cerr << "ERROR: Mesh has no vertices" << std::endl;
        return;
    }
    
    // Process vertices (with bounds checking)
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        // Position
        if(mesh->mVertices) {
            vertex.Position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
        }
        
        // Normals (check if they exist)
        if(mesh->mNormals) {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        
        // Texture coordinates (check if they exist)
        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        
        vertices.push_back(vertex);
    }
    
    // Process indices with safety check
    if(mesh->mNumFaces == 0) {
        std::cerr << "WARNING: Mesh has no faces" << std::endl;
    } else {
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }
    
    // Process materials with safety checks
    if(scene->mNumMaterials > 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if(material) {
            aiString str;
            if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                if(material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS) {
                    std::string texturePath = "resources/textures/" + std::string(str.C_Str());
                    std::cout << "Loading texture: " << texturePath << std::endl;
                    textures.push_back(Texture(texturePath.c_str(), "texture_diffuse"));
                }
            }
        }
    }
    
    setupMesh();
}