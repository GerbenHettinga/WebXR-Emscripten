#version 300 es
in vec3 vPos;

uniform mat4 matrix;
uniform mat4 proj_matrix;

out vec3 pos;
void main() {
    pos = vPos;

    gl_PointSize = 5.0f;
    gl_Position = proj_matrix * matrix * vec4(pos, 1.0);
}
