#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform float     time;
uniform vec3      cameraPos;
uniform sampler2D waterTexture;

uniform float     fogDensity;
uniform vec3      fogColorLow;
uniform vec3      fogColorHigh;
uniform float     minHeight;
uniform float     maxHeight;

uniform float     edgeStart;
uniform float     edgeEnd;
uniform float     edgeStrength;

void main() {
    // 1) compute your moving UVs
    vec2 mv = TexCoords + vec2(
        sin(time * 0.3) * 0.015,
        cos(time * 0.2 + TexCoords.x * 2.0) * 0.015
    );

    // 2) if outside [0,1], drop the fragment entirely
    if (mv.x < 0.0 || mv.x > 1.0 ||
        mv.y < 0.0 || mv.y > 1.0) {
        discard;
    }

    // 3) your existing water look
    float wave = sin(TexCoords.y * 4.0 + time * 1.2) * 0.05;
    vec3 wc = mix(vec3(0.12,0.22,0.38), vec3(0.18,0.45,0.7), TexCoords.y + wave);
    float rip = sin(TexCoords.x * 8.0 + time * 1.5) * 0.02;
    wc += rip * 0.15;
    vec3 texC  = texture(waterTexture, mv).rgb;
    vec3 color = mix(wc, texC, 0.6) * 0.7;

    // 4) distance‑based fog
    float dist      = length(cameraPos - FragPos);
    float fogFactor = clamp(1.0 - exp(-fogDensity * dist), 0.0, 1.0);

    // 5) height‑based fog color
    float t    = clamp((FragPos.y - minHeight)/(maxHeight - minHeight), 0.0, 1.0);
    vec3 fogC  = mix(fogColorLow, fogColorHigh, t);
    color      = mix(color, fogC, fogFactor);

    // 6) edge darkening
    float hd = length(cameraPos.xz - FragPos.xz);
    float ef = smoothstep(edgeStart, edgeEnd, hd);
    color    *= 1.0 - ef * edgeStrength;

    FragColor = vec4(color, 0.85);
}
