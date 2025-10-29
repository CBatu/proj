#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in mat4 aModel;
layout(location = 5) in vec4 aColor;

out vec4 vColor;
uniform mat4 uViewProj;

void main() {
    vColor = aColor;
    gl_Position = uViewProj * aModel * vec4(aPos, 1.0);
}
