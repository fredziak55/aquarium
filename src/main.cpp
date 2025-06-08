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
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 3.0f, 10.0f));
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;
bool  firstMouse = true;

// Timing
float deltaTime = 0.0f, lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos1(2.0f,2.0f,2.0f), lightPos2(-2.0f,2.0f,-2.0f);

void framebuffer_size_callback(GLFWwindow* w,int width,int height){
    glViewport(0,0,width,height);
}
void mouse_callback(GLFWwindow* w,double xpos,double ypos){
    if(firstMouse){ lastX=xpos; lastY=ypos; firstMouse=false; }
    float xoff=xpos-lastX, yoff=lastY-ypos;
    lastX=xpos; lastY=ypos;
    camera.ProcessMouseMovement(xoff,yoff);
}
void scroll_callback(GLFWwindow* w,double xoff,double yoff){
    camera.ProcessMouseScroll(yoff);
}
void processInput(GLFWwindow* w){
    if(glfwGetKey(w,GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(w,true);
    if(glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS) camera.ProcessKeyboard(BACKWARD,deltaTime);
    if(glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS) camera.ProcessKeyboard(LEFT,    deltaTime);
    if(glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS) camera.ProcessKeyboard(RIGHT,   deltaTime);
}

int main(){
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Aquarium",NULL,NULL);
    if(!window){ std::cerr<<"Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cerr<<"Failed to init GLAD\n"; return -1; }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader shader    ("resources/shaders/vertex.glsl",       "resources/shaders/fragment.glsl");
    Shader waterShader("resources/shaders/water_vertex.glsl","resources/shaders/water_fragment.glsl");
    Shader lightShader("resources/shaders/light_vertex.glsl","resources/shaders/light_fragment.glsl");
    Shader sandShader ("resources/shaders/sand_vertex.glsl", "resources/shaders/sand_fragment.glsl");

    // Scene objects
    std::vector<Fish> fishes = {
        Fish("resources/models/fish.obj",{1,3,1},{0.5f,0,0.5f},0.5f),
        Fish("resources/models/fish.obj",{-1,3.2f,-1},{-0.3f,0.1f,-0.2f},0.3f),
        Fish("resources/models/fish.obj",{0,3.5f,0},{0.2f,-0.1f,0.3f},0.4f),
        Fish("resources/models/fish.obj",{0,3.7f,2},{-0.2f,-0.1f,-0.4f},0.45f),
        Fish("resources/models/fish.obj",{0,4,-2},{0.1f,0.1f,0.4f},1.0f)
    };
    std::vector<Plant> plants = {
        Plant("resources/models/plant.obj",{-3.5f,-1, -3}),
        Plant("resources/models/plant.obj",{ 3.5f,-1, -3}),
        Plant("resources/models/plant.obj",{-3.5f,-1,  3}),
        Plant("resources/models/plant.obj",{ 3.5f,-1,  3})
    };
    Model coral("resources/models/coral.obj"), rock("resources/models/rock.obj");
    Water water(10.0f);
    glm::vec3 tankMin(-4,-1,-4), tankMax(4,10,4);

    // Textures
    unsigned int sandTexture;
    glGenTextures(1,&sandTexture);
    glBindTexture(GL_TEXTURE_2D,sandTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    int w,h,c; unsigned char* d=stbi_load("resources/textures/sand.jpg",&w,&h,&c,0);
    if(d){ glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,d); glGenerateMipmap(GL_TEXTURE_2D); }
    stbi_image_free(d);

    unsigned int waterTexture;
    glGenTextures(1,&waterTexture);
    glBindTexture(GL_TEXTURE_2D,waterTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    int ww,hh,cc; unsigned char* wd=stbi_load("resources/textures/water.png",&ww,&hh,&cc,0);
    if(wd){ GLenum fmt=(cc==4?GL_RGBA:GL_RGB);
        glTexImage2D(GL_TEXTURE_2D,0,fmt,ww,hh,0,fmt,GL_UNSIGNED_BYTE,wd);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(wd);

    // VAOs
    unsigned int waterVAO,waterVBO,waterEBO;
    { float verts[]={-1,-1,0,0,0, 1,-1,0,1,0, 1,1,0,1,1, -1,1,0,0,1};
      unsigned int idx[]={0,1,2,2,3,0};
      glGenVertexArrays(1,&waterVAO); glGenBuffers(1,&waterVBO); glGenBuffers(1,&waterEBO);
      glBindVertexArray(waterVAO);
      glBindBuffer(GL_ARRAY_BUFFER,waterVBO); glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,waterEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STATIC_DRAW);
      glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
      glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
      glBindVertexArray(0);
    }
    unsigned int sandVAO,sandVBO,sandEBO;
    { float verts[]={-20,-1,-20,0,0, 20,-1,-20,1,0, 20,-1,20,1,1, -20,-1,20,0,1};
      unsigned int idx[]={0,1,2,2,3,0};
      glGenVertexArrays(1,&sandVAO); glGenBuffers(1,&sandVBO); glGenBuffers(1,&sandEBO);
      glBindVertexArray(sandVAO);
      glBindBuffer(GL_ARRAY_BUFFER,sandVBO); glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sandEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STATIC_DRAW);
      glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
      glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
      glBindVertexArray(0);
    }
    unsigned int lightVAO,lightVBO,lightEBO;
    { float verts[]={-0.1,-0.1,-0.1, 0.1,-0.1,-0.1, 0.1,0.1,-0.1, -0.1,0.1,-0.1,
                     -0.1,-0.1,0.1,  0.1,-0.1,0.1,  0.1,0.1,0.1,  -0.1,0.1,0.1};
      unsigned int idx[]={0,1,2,2,3,0,4,5,6,6,7,4,0,1,5,5,4,0,2,3,7,7,6,2,0,3,7,7,4,0,1,2,6,6,5,1};
      glGenVertexArrays(1,&lightVAO); glGenBuffers(1,&lightVBO); glGenBuffers(1,&lightEBO);
      glBindVertexArray(lightVAO);
      glBindBuffer(GL_ARRAY_BUFFER,lightVBO); glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,lightEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STATIC_DRAW);
      glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
      glBindVertexArray(0);
    }

    // Render loop
    while(!glfwWindowShouldClose(window)){
        float current=glfwGetTime();
        deltaTime=current-lastFrame; lastFrame=current;
        processInput(window);
        glClearColor(0.1f,0.1f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Draw fishes, plants, coral
        shader.use();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);
        glm::mat4 view= camera.GetViewMatrix();
        shader.setMat4("projection",proj);
        shader.setMat4("view",view);
        shader.setVec3("lightPos1",lightPos1);
        shader.setVec3("lightPos2",lightPos2);
        shader.setVec3("viewPos",camera.Position);
        shader.setVec3("lightColor1",1.0f,1.0f,1.0f);
        shader.setVec3("lightColor2",0.5f,0.5f,1.0f);
        shader.setFloat("fogDensity",0.01f);
        shader.setVec3("fogColor",glm::vec3(0.0f,0.60f,0.70f));
        shader.setFloat("fogMaxDistance",50.0f);
        shader.setFloat("mipBiasScale",0.02f);

        for(auto& f:fishes) f.Update(deltaTime,tankMin,tankMax);
        // fish-fish collision
        for(size_t i=0;i<fishes.size();++i)
            for(size_t j=i+1;j<fishes.size();++j){
                glm::vec3 d=fishes[i].GetPosition()-fishes[j].GetPosition();
                float dist=glm::length(d);
                if(dist<0.3f && dist>0.0f){
                    glm::vec3 sep=glm::normalize(d)*0.1f;
                    fishes[i].SetDirection(fishes[i].GetDirection()+sep);
                    fishes[j].SetDirection(fishes[j].GetDirection()-sep);
                }
            }
        for(auto& f:fishes) f.Draw(shader);

        for(auto& p:plants){ p.Update(deltaTime); p.Draw(shader); }

        float sway=sin(current*0.5f)*5.0f;
        std::vector<glm::vec3> cps={{-1,-0.5f,0},{2,-0.5f,-1},{-2,-0.5f,1},{0,-0.5f,2}};
        for(auto& pos:cps){
            glm::mat4 m=glm::translate(glm::mat4(1.0f),pos);
            m=glm::rotate(m,glm::radians(270.0f),glm::vec3(1,0,0));
            m=glm::rotate(m,glm::radians(sway),glm::vec3(0,1,0));
            m=glm::scale(m,glm::vec3(0.05f));
            shader.setMat4("model",m);
            coral.Draw(shader);
        }

        // Draw sand
        sandShader.use();
        sandShader.setMat4("projection",proj);
        sandShader.setMat4("view",     view);
        sandShader.setMat4("model",    glm::mat4(1.0f));
        sandShader.setFloat("fogDensity",  0.04f);
        sandShader.setVec3("fogColorLow",   glm::vec3(0.02f,0.08f,0.12f));
        sandShader.setVec3("fogColorHigh",  glm::vec3(0.90f,0.85f,0.70f));
        sandShader.setFloat("minHeight",   -1.0f);
        sandShader.setFloat("maxHeight",    3.0f);
        sandShader.setFloat("edgeStart",   15.0f);
        sandShader.setFloat("edgeEnd",     30.0f);
        sandShader.setFloat("edgeStrength",0.8f);
        sandShader.setFloat("distStart",    5.0f);
        sandShader.setFloat("distEnd",     30.0f);
        sandShader.setFloat("distStrength",0.4f);
        sandShader.setVec3("sandTintColor",    glm::vec3(0.00f,0.80f,0.70f));
        sandShader.setFloat("sandTintStrength",0.15f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,sandTexture);
        sandShader.setInt("sandTexture",0);
        glBindVertexArray(sandVAO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        // Draw water walls
        waterShader.use();
        waterShader.setMat4("projection",proj);
        waterShader.setMat4("view",      view);
        waterShader.setFloat("time",     current);
        waterShader.setVec3 ("cameraPos",camera.Position);
        waterShader.setFloat("fogDensity", 0.01f);
        waterShader.setVec3 ("fogColorLow",   glm::vec3(0.00f,0.30f,0.40f));
        waterShader.setVec3 ("fogColorHigh",  glm::vec3(0.30f,0.90f,0.85f));
        waterShader.setFloat("minHeight",   -1.0f);
        waterShader.setFloat("maxHeight",    5.0f);
        waterShader.setFloat("edgeStart",   10.0f);
        waterShader.setFloat("edgeEnd",     20.0f);
        waterShader.setFloat("edgeStrength",0.2f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,waterTexture);
        waterShader.setInt("waterTexture",0);
        glBindVertexArray(waterVAO);
        // back
        glm::mat4 wm=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-20));
        wm=glm::scale(wm,glm::vec3(30,20,1));
        waterShader.setMat4("model",wm);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // left
        wm=glm::translate(glm::mat4(1.0f),glm::vec3(-20,0,0));
        wm=glm::rotate(wm,glm::radians(90.0f),glm::vec3(0,1,0));
        wm=glm::scale(wm,glm::vec3(30,20,1));
        waterShader.setMat4("model",wm);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // right
        wm=glm::translate(glm::mat4(1.0f),glm::vec3(20,0,0));
        wm=glm::rotate(wm,glm::radians(-90.0f),glm::vec3(0,1,0));
        wm=glm::scale(wm,glm::vec3(30,20,1));
        waterShader.setMat4("model",wm);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // ceiling
        wm=glm::translate(glm::mat4(1.0f),glm::vec3(0,20,0));
        wm=glm::rotate(wm,glm::radians(90.0f),glm::vec3(1,0,0));
        wm=glm::scale(wm,glm::vec3(30,30,1));
        waterShader.setMat4("model",wm);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // front
        wm=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,20));
        wm=glm::scale(wm,glm::vec3(30,20,1));
        waterShader.setMat4("model",wm);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        // Draw lights
        lightShader.use();
        lightShader.setMat4("projection",proj);
        lightShader.setMat4("view",      view);
        glm::mat4 lm=glm::translate(glm::mat4(1.0f),lightPos1);
        lm=glm::scale(lm,glm::vec3(0.5f));
        lightShader.setMat4("model",lm);
        lightShader.setVec3("lightColor",1.0f,1.0f,1.0f);
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
        lm=glm::translate(glm::mat4(1.0f),lightPos2);
        lm=glm::scale(lm,glm::vec3(0.5f));
        lightShader.setMat4("model",lm);
        lightShader.setVec3("lightColor",0.5f,0.5f,1.0f);
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
