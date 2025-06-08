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
    
    // Load models
    std::vector<Fish> fishes;
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(0.5f, 0.0f, 0.5f), 0.5f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(-1.0f, 3.2f, -1.0f), glm::vec3(-0.3f, 0.1f, -0.2f), 0.3f);
    fishes.emplace_back("resources/models/fish.obj", glm::vec3(0.0f, 3.5f, 0.0f), glm::vec3(0.2f, -0.1f, 0.4f), 0.4f);

    std::vector<Plant> plants;
    // Shift plant positions further away from coral.
    // Coral is drawn at (-1.0f, -0.5f, 0.0f) so we can move plants farther out.
    // Make sure the new positions still lie within your tank boundaries.
    plants.emplace_back("resources/models/plant.obj", glm::vec3(-1.5f, -1.0f, -2.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(1.5f, -1.0f, -2.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(-1.5f, -1.0f, 2.0f));
    plants.emplace_back("resources/models/plant.obj", glm::vec3(1.5f, -1.0f, 2.0f));
        
    Model coral("resources/models/coral.obj");
    Model rock("resources/models/rock.obj");
    
    Water water(10.0f);
    
    // Tank boundaries
    glm::vec3 tankMin(-4.0f, -1.0f, -4.0f);
    glm::vec3 tankMax(4.0f, 10.0f, 4.0f);
    
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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 0.0f)); // base position
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
        // Add the sway rotation; adjust axis/vector based on desired effect
        model = glm::rotate(model, glm::radians(swayAngle), glm::vec3(0.0f, 1.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(0.05f));
        shader.setMat4("model", model);
        coral.Draw(shader);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        shader.setMat4("model", model);
        rock.Draw(shader);
        
        // Draw water surface
        waterShader.use();
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("view", view);
        waterShader.setVec3("cameraPos", camera.Position);
        waterShader.setVec3("lightPos1", lightPos1);
        waterShader.setVec3("lightPos2", lightPos2);
        water.Draw(waterShader, currentFrame);
        
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