#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float time;
uniform vec3 cameraPos;
uniform sampler2D waterTexture;

void main() {
    // Smoother movement: lower frequency and amplitude
    vec2 movingTexCoords = TexCoords + vec2(
        sin(time * 0.3) * 0.015,
        cos(time * 0.2 + TexCoords.x * 2.0) * 0.015
    );

    // Smoother procedural color
    float wave = sin(TexCoords.y * 4.0 + time * 1.2) * 0.05;
    vec3 waterColor = mix(
        vec3(0.12, 0.22, 0.38),
        vec3(0.18, 0.45, 0.7),
        TexCoords.y + wave
    );
    float ripple = sin(TexCoords.x * 8.0 + time * 1.5) * 0.02;
    waterColor += ripple * 0.15;

    // Sample texture with moving coordinates
    vec4 texColor = texture(waterTexture, movingTexCoords);

    // Blend procedural and texture color (more texture, less harsh)
    vec3 finalColor = mix(waterColor, texColor.rgb, 0.6);

    FragColor = vec4(finalColor, 0.85);
}