#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D sandTexture;
uniform vec3      viewPos;

// height‑fog
uniform float fogDensity;
uniform vec3  fogColorLow;
uniform vec3  fogColorHigh;
uniform float minHeight;
uniform float maxHeight;

// edge darkening
uniform float edgeStart;
uniform float edgeEnd;
uniform float edgeStrength;

// distance darkening
uniform float distStart;
uniform float distEnd;
uniform float distStrength;

// **new turquoise tint**
uniform vec3  sandTintColor;
uniform float sandTintStrength;

void main() {
    // 1) base + height‑fog
    vec3 color    = texture(sandTexture, TexCoord).rgb;
    float dCam    = length(viewPos - FragPos);
    float fogF    = clamp(1.0 - exp(-fogDensity * dCam), 0.0, 1.0);
    float ht      = clamp((FragPos.y - minHeight)/(maxHeight - minHeight), 0.0, 1.0);
    vec3 tFog     = mix(fogColorLow, fogColorHigh, ht);
    color         = mix(color, tFog, fogF);

    // 2) edge‑darkening
    float hDist   = length(viewPos.xz - FragPos.xz);
    float eF      = smoothstep(edgeStart, edgeEnd, hDist);
    color        *= 1.0 - eF * edgeStrength;

    // 3) distance‑darkening
    float dF      = smoothstep(distStart, distEnd, dCam);
    color        *= 1.0 - dF * distStrength;

    // 4) turquoise tint
    color = mix(color, sandTintColor, sandTintStrength);

    FragColor = vec4(color, 1.0);
}
