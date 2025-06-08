#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float time;
uniform vec3 cameraPos;

void main() {
    // Create vertical wave movement (like flowing water)
    float wave = sin(TexCoords.y * 10.0 + time * 2.0) * 0.1;
    
    // Ocean-like color with depth variation
    vec3 waterColor = mix(
        vec3(0.1, 0.2, 0.4),  // Darker blue at the bottom
        vec3(0.2, 0.5, 0.8),  // Lighter blue at the top
        TexCoords.y            // Gradient from bottom to top
    );
    
    // Add subtle horizontal ripples
    float ripple = sin(TexCoords.x * 20.0 + time * 3.0) * 0.05;
    waterColor += ripple * 0.2;
    
    // Make it slightly transparent for a "watery" look
    FragColor = vec4(waterColor, 0.85);
}