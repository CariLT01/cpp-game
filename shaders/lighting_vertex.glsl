#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uViewPosition;

layout (location = 0) in vec3 aPosition;

out vec3 vViewPosition;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vViewPosition = uViewPosition;

}