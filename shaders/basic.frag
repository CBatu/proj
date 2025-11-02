#version 330 core

in vec4 vColor;
in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

struct Light {
    int type;           // 0 = directional, 1 = point
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform int uLightCount;
uniform Light uLights[8];
uniform vec3 uViewPos;

// Yeni
uniform sampler2D uDiffuseTex;
uniform int uUseTexture; // 0 = renk, 1 = texture

void main() {
    vec3 baseColor = (uUseTexture == 1) ? texture(uDiffuseTex, vTexCoord).rgb : vColor.rgb;

    vec3 normal = normalize(vNormal);
    vec3 result = vec3(0.0);

    if (uLightCount == 0) {
        FragColor = vec4(baseColor, vColor.a);
        return;
    }

    for (int i = 0; i < uLightCount; i++) {
        Light light = uLights[i];
        vec3 lightDir;
        if (light.type == 0) lightDir = normalize(-light.direction);
        else                 lightDir = normalize(light.position - vFragPos);

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * light.color * light.intensity;
        result += diffuse * baseColor;
    }

    FragColor = vec4(result, vColor.a);
}
