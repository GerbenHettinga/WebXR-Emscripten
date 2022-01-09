#version 300 es
precision mediump float;
in vec3 pos;

uniform vec3 vertexCol;

out vec4 fCol;
void main() {
    fCol = vec4(vertexCol, 1.0);
}
