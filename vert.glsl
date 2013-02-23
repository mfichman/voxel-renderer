
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

attribute vec3 pos;
attribute vec3 normal;
attribute vec2 tex;

varying float color;

void main() {
    gl_Position = proj * view * model * vec4(pos, 1);
    vec3 light = normalize(vec3(1, 5, 2));
    color = dot(light, normal);
}
