#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 aModel;
layout(location = 5) in vec4 aColor;
layout(location = 6) in vec2 aTexCoord; // <-- yeni

out vec4 vColor;
out vec3 vFragPos;
out vec3 vNormal;
out vec2 vTexCoord; // <-- fragment shader'a gönderilecek

uniform mat4 uViewProj;

void main() {
    vColor = aColor;
    vTexCoord = aTexCoord; // <-- UV yi gönderiyoruz

    vec4 worldPos = aModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = normalize(mat3(aModel) * normalize(aPos));

    gl_Position = uViewProj * worldPos;
}
