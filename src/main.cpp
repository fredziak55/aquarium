#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Fish.h"
#include "Plant.h"
#include "Water.h"
#include "Model.h"

#include <iostream>
#include <vector>

#include <stb_image.h>

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 3.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos1(2.0f, 2.0f, 2.0f);
glm::vec3 lightPos2(-2.0f, 2.0f, -2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Aquarium", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Build and compile shaders
    Shader shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");
    Shader waterShader("resources/shaders/water_vertex.glsl", "resources/shaders/water_fragment.glsl");
    Shader lightShader("resources/shaders/light_vertex.glsl", "resources/shaders/light_fragment.glsl");
    Shader sandShader("resources/shaders/sand_vertex.glsl", "resources/shaders/sand_fragment.glsl");
    
    // Load models
    std::vector<Fish> fishes;
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(0.5f, 0.0f, 0.5f), 0.5f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(-1.0f, 3.2f, -1.0f), glm::vec3(-0.3f, 0.1f, -0.2f), 0.3f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.2f, -0.1f, 0.3f), 0.4f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(0.0f, 3.7f, 2.0f), glm::vec3(-0.2f, -0.1f, -0.4f), 0.45f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(0.0f, 4.0f, -2.0f), glm::vec3(0.1f, 0.1f, 0.4f), 1.0f);

    std::vector<Plant> plants;
    // Shift plant positions further away from coral.
    // Coral is drawn at (-1.0f, -0.5f, 0.0f) so we can move plants farther out.
    // Make sure the new positions still lie within your tank boundaries.
    plants.emplace_back("resources/models/plant.obj", glm::vec3(-3.5f, -1.0f, -3.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(3.5f, -1.0f, -3.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(-3.5f, -1.0f, 3.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(3.5f, -1.0f, 3.0f));
        
    Model coral("resources/models/coral.obj");
    Model rock("resources/models/rock.obj");
    
    Water water(10.0f);
    
    // Tank boundaries
    glm::vec3 tankMin(-4.0f, -1.0f, -4.0f);
    glm::vec3 tankMax(4.0f, 10.0f, 4.0f);
    
    unsigned int sandTexture;
    glGenTextures(1, &sandTexture);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image
    int width, height, nrChannels;
    unsigned char *data = stbi_load("resources/textures/sand.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load sand texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int waterTexture;
    glGenTextures(1, &waterTexture);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image
    int wWidth, wHeight, wChannels;
    unsigned char *wData = stbi_load("resources/textures/water.png", &wWidth, &wHeight, &wChannels, 0);
    if (wData) {
        GLenum format = (wChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, wWidth, wHeight, 0, format, GL_UNSIGNED_BYTE, wData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load water texture" << std::endl;
    }
    stbi_image_free(wData);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Input
        processInput(window);
        
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        shader.use();
        
        // Pass projection matrix to shader
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        
        // Camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        
        // Set light properties
        shader.setVec3("lightPos1", lightPos1);
        shader.setVec3("lightPos2", lightPos2);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightColor1", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightColor2", 0.5f, 0.5f, 1.0f);
        
        // Draw aquarium objects
        for (auto& fish : fishes) {
            fish.Update(deltaTime, tankMin, tankMax);
            fish.Draw(shader);
        }
        
        for (auto& plant : plants) {
            plant.Update(deltaTime);
            plant.Draw(shader);
        }

                // After updating each fish’s position:
        float fishCollisionRadius = 0.3f; // Adjust based on fish size
        for (size_t i = 0; i < fishes.size(); i++) {
            for (size_t j = i + 1; j < fishes.size(); j++) {
                glm::vec3 diff = fishes[i].GetPosition() - fishes[j].GetPosition();
                float distance = glm::length(diff);
                if(distance < fishCollisionRadius && distance > 0.0f) {
                    glm::vec3 separation = glm::normalize(diff);
                    // Adjust each fish's direction slightly away from the other
                    fishes[i].SetDirection(fishes[i].GetDirection() + separation * 0.1f);
                    fishes[j].SetDirection(fishes[j].GetDirection() - separation * 0.1f);
                }
            }
        }

        // Draw static objects
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f)); // coral is now higher
        // model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
        // model = glm::scale(model, glm::vec3(0.05f));
        // shader.setMat4("model", model);
        // coral.Draw(shader);

        float swayAngle = sin(currentFrame * 0.5f) * 5.0f; // 5 degrees max sway

        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f)); // base position
        // model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
        // // Add the sway rotation; adjust axis/vector based on desired effect
        // model = glm::rotate(model, glm::radians(swayAngle), glm::vec3(0.0f, 1.0f, 0.0f)); 
        // model = glm::scale(model, glm::vec3(0.05f));
        // shader.setMat4("model", model);
        // coral.Draw(shader);

        std::vector<glm::vec3> coralPositions = {
            glm::vec3(-1.0f, -0.5f, 0.0f),
            glm::vec3(2.0f, -0.5f, -1.0f),
            glm::vec3(-2.0f, -0.5f, 1.0f),
            glm::vec3(0.0f, -0.5f, 2.0f)
        };

        for (const auto& position : coralPositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(swayAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.05f));
            shader.setMat4("model", model);
            coral.Draw(shader);
        }
        
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(100.0f));
        // shader.setMat4("model", model);
        // rock.Draw(shader);
        
         // Simple quad vertices (x, y, z, u, v)
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Set up VAO/VBO/EBO for the water quad
        unsigned int waterVAO, waterVBO, waterEBO;
        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);
        glGenBuffers(1, &waterEBO);

        glBindVertexArray(waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Define full-screen quad vertices in NDC and their UV coordinates
                // Draw water surface in background
        waterShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTexture);
        waterShader.setInt("waterTexture", 0);
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("view", view);
        waterShader.setVec3("cameraPos", camera.Position);
        waterShader.setVec3("lightPos1", lightPos1);
        waterShader.setVec3("lightPos2", lightPos2);

        // Back wall
        glm::mat4 waterModel = glm::mat4(1.0f);
        waterModel = glm::translate(waterModel, glm::vec3(0.0f, 0.0f, -20.0f)); // Move far back
        waterModel = glm::scale(waterModel, glm::vec3(30.0f, 20.0f, 1.0f));    // Make it large (width, height, depth)
        waterShader.setMat4("model", waterModel);
        waterShader.setFloat("time", currentFrame);
        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Left wall
        waterModel = glm::mat4(1.0f);
        waterModel = glm::translate(waterModel, glm::vec3(-20.0f, 0.0f, 0.0f)); // Move to the left
        waterModel = glm::rotate(waterModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate to face right
        waterModel = glm::scale(waterModel, glm::vec3(30.0f, 20.0f, 1.0f));    // Make it large
        waterShader.setMat4("model", waterModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Right wall
        waterModel = glm::mat4(1.0f);
        waterModel = glm::translate(waterModel, glm::vec3(20.0f, 0.0f, 0.0f)); // Move to the right
        waterModel = glm::rotate(waterModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate to face left
        waterModel = glm::scale(waterModel, glm::vec3(30.0f, 20.0f, 1.0f));    // Make it large
        waterShader.setMat4("model", waterModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Ceiling
        waterModel = glm::mat4(1.0f);
        waterModel = glm::translate(waterModel, glm::vec3(0.0f, 20.0f, 0.0f)); // Move to the top
        waterModel = glm::rotate(waterModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to face downward
        waterModel = glm::scale(waterModel, glm::vec3(30.0f, 30.0f, 1.0f));    // Make it large
        waterShader.setMat4("model", waterModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        waterModel = glm::mat4(1.0f);
        waterModel = glm::translate(waterModel, glm::vec3(0.0f, 0.0f, 20.0f)); // Move in front of the camera
        waterModel = glm::scale(waterModel, glm::vec3(30.0f, 20.0f, 1.0f));    // Make it large (width, height, depth)
        waterShader.setMat4("model", waterModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        waterShader.setMat4("model", waterModel);
        waterShader.setFloat("time", currentFrame);

        glBindVertexArray(0);

       

        // Draw the water quad
        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Clean up
        glDeleteVertexArrays(1, &waterVAO);
        glDeleteBuffers(1, &waterVBO);
        glDeleteBuffers(1, &waterEBO);

        float sandVertices[] = {
            // positions          // texture coords
            -20.0f, -1.0f, -20.0f,   0.0f, 0.0f,  // Bottom-left corner
            20.0f, -1.0f, -20.0f,   1.0f, 0.0f,  // Bottom-right corner
            20.0f, -1.0f,  20.0f,   1.0f, 1.0f,  // Top-right corner
            -20.0f, -1.0f,  20.0f,   0.0f, 1.0f   // Top-left corner
        };
        unsigned int sandIndices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Set up VAO/VBO/EBO for the sand quad
        unsigned int sandVAO, sandVBO, sandEBO;
        glGenVertexArrays(1, &sandVAO);
        glGenBuffers(1, &sandVBO);
        glGenBuffers(1, &sandEBO);

        glBindVertexArray(sandVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sandVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sandVertices), sandVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sandEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sandIndices), sandIndices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coord attribute (not used but defined for future use)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

          //light cube vertices and indices
        // These vertices define a small cube that will represent the light source in the scene.
        float lightVertices[] = {
            // positions          
            -0.1f, -0.1f, -0.1f,
            0.1f, -0.1f, -0.1f,
            0.1f,  0.1f, -0.1f,
            -0.1f,  0.1f, -0.1f,
            -0.1f, -0.1f,  0.1f,
            0.1f, -0.1f,  0.1f,
            0.1f,  0.1f,  0.1f,
            -0.1f,  0.1f,  0.1f
        };
        unsigned int lightIndices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 1, 5, 5, 4, 0,
            2, 3, 7, 7, 6, 2,
            0, 3, 7, 7, 4, 0,
            1, 2, 6, 6, 5, 1
        };

        unsigned int lightVAO, lightVBO, lightEBO;
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);
        glGenBuffers(1, &lightEBO);

        glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightIndices), lightIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        sandShader.use();
        sandShader.setMat4("projection", projection);
        sandShader.setMat4("view", view);

        glm::mat4 sandModel = glm::mat4(1.0f);
        sandModel = glm::translate(sandModel, glm::vec3(0.0f, 0.5f, 0.0f)); // Position at the bottom
        sandShader.setMat4("model", sandModel);

        // Bind sand texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        sandShader.setInt("sandTexture", 0);

        glBindVertexArray(sandVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // Render light source 1
        glm::mat4 lightModel1 = glm::mat4(1.0f);
        lightModel1 = glm::translate(lightModel1, lightPos1); // Position at lightPos1
        lightModel1 = glm::scale(lightModel1, glm::vec3(0.5f)); // Scale down to make it small
        lightShader.setMat4("model", lightModel1);
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Bright white color

        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Render light source 2
        glm::mat4 lightModel2 = glm::mat4(1.0f);
        lightModel2 = glm::translate(lightModel2, lightPos2); // Position at lightPos2
        lightModel2 = glm::scale(lightModel2, glm::vec3(0.5f)); // Scale down to make it small
        lightShader.setMat4("model", lightModel2);
        lightShader.setVec3("lightColor", glm::vec3(0.5f, 0.5f, 1.0f)); // Slightly blue color

        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}