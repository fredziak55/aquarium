#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main() {
    // Simple wave animation
    float waveHeight = 0.05 * sin(aPos.x * 2.0 + time * 2.0) * cos(aPos.z * 2.0 + time * 2.0);
    vec3 pos = aPos + vec3(0.0, waveHeight, 0.0);
    
    FragPos = vec3(model * vec4(pos, 1.0));
    TexCoords = aTexCoords;
    
    // Calculate normal for waves
    float dx = 0.1 * 2.0 * cos(aPos.x * 2.0 + time * 2.0) * cos(aPos.z * 2.0 + time * 2.0);
    float dz = 0.1 * 2.0 * sin(aPos.x * 2.0 + time * 2.0) * sin(aPos.z * 2.0 + time * 2.0);
    Normal = normalize(vec3(-dx, 1.0, -dz));
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
}