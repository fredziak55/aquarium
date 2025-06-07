#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform vec3 cameraPos;
uniform vec3 lightPos1;
uniform vec3 lightPos2;

void main() {
    vec3 waterColor = vec3(0.1, 0.3, 0.5);
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    
    // Combine with water color
    vec3 color = mix(waterColor, texColor, 0.3);
    
    // Lighting
    vec3 norm = normalize(Normal);
    
    // Light 1
    vec3 lightDir1 = normalize(lightPos1 - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * vec3(1.0);
    
    // Light 2
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * vec3(0.5, 0.5, 1.0);
    
    // Specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 64);
    vec3 specular1 = 0.5 * spec1 * vec3(1.0);
    
    // Fresnel effect
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 2.0);
    vec3 fresnelColor = mix(color, vec3(1.0), fresnel * 0.7);
    
    vec3 result = (0.2 + diffuse1 + diffuse2 + specular1) * fresnelColor;
    FragColor = vec4(result, 0.7); // Semi-transparent
}