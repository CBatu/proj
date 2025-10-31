#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 aModel;
layout(location = 5) in vec4 aColor;

out vec4 vColor;
out vec3 vFragPos;
out vec3 vNormal;

uniform mat4 uViewProj;

void main() {
    vColor = aColor;

    // Dünya uzayındaki konum
    vec4 worldPos = aModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;

    // Normal — küp için basit haliyle sadece pozisyondan türetiyoruz
    vNormal = normalize(mat3(aModel) * normalize(aPos));


    gl_Position = uViewProj * worldPos;
}
