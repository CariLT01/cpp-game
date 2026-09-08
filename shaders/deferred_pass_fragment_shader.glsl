#version 460 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D uLightTexture;

in vec2 vTexCoord;

out vec4 FragColor;

void main() {

    vec3 light = texture(uLightTexture, vTexCoord).rgb;

    FragColor = vec4(pow(light, vec3(1.0 / 2.2)), 1.0);
}