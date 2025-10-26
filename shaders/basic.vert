#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in mat4 aModel; // instance

uniform mat4 uViewProj;
out vec4 vColor;

void main() {
    gl_Position = uViewProj * aModel * vec4(aPos, 1.0);
    vColor = aColor;
}
