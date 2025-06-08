#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3      lightPos1;
uniform vec3      lightPos2;
uniform vec3      viewPos;
uniform vec3      lightColor1;
uniform vec3      lightColor2;

// ← new fog uniforms
uniform float     fogDensity;    // e.g. 0.04
uniform vec3      fogColor;      // e.g. vec3(0.0, 0.1, 0.2)

void main() {
    // --- lighting as before ---
    vec3 ambient = 0.2 * (lightColor1 + lightColor2);

    vec3 N  = normalize(Normal);
    vec3 L1 = normalize(lightPos1 - FragPos);
    vec3 L2 = normalize(lightPos2 - FragPos);
    float d1 = max(dot(N, L1), 0.0);
    float d2 = max(dot(N, L2), 0.0);
    vec3 diffuse  = d1 * lightColor1 + d2 * lightColor2;

    vec3 V  = normalize(viewPos - FragPos);
    vec3 R1 = reflect(-L1, N);
    vec3 R2 = reflect(-L2, N);
    float s1 = pow(max(dot(V, R1), 0.0), 32);
    float s2 = pow(max(dot(V, R2), 0.0), 32);
    vec3 specular = 0.5 * (s1*lightColor1 + s2*lightColor2);

    vec3 baseColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 color = (ambient + diffuse + specular) * baseColor;

    // --- exponential fog ---
    float dist = length(viewPos - FragPos);
    // 1.0 - exp(-density * dist) gives fogFactor from 0 (near) to 1 (far)
    float fogFactor = clamp(1.0 - exp(-fogDensity * dist), 0.0, 1.0);
    color = mix(color, fogColor, fogFactor);

    FragColor = vec4(color, 1.0);
}
