#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 viewPos;
uniform vec3 lightColor1;
uniform vec3 lightColor2;

void main() {
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * (lightColor1 + lightColor2);
    
    // Diffuse 1
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(lightPos1 - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * lightColor1;
    
    // Diffuse 2
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * lightColor2;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32);
    vec3 specular1 = specularStrength * spec1 * lightColor1;
    
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32);
    vec3 specular2 = specularStrength * spec2 * lightColor2;
    
    vec3 result = (ambient + diffuse1 + diffuse2 + specular1 + specular2) * texture(texture_diffuse1, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
}