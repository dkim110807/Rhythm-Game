#version 330 core

out vec4 out_color;

uniform int render;
uniform vec4 color;

void main() {
    if (render == 0) discard;
    out_color = color;
}