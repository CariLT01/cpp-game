#version 460 core

uniform sampler2D uTextAtlas;

in vec2 vUv;

out vec4 FragColor;

void main() {
    vec4 color = texture(uTextAtlas, vUv);
    if (color.a <= 0.1) discard;
    FragColor = color;
}