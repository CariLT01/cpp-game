#version 460 core

layout (location = 0) out vec3 lightTexture;

uniform vec3 uLightPosition;
uniform float uLightRadius;
uniform vec3 uLightColor;
uniform float uLightIntensity;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

in vec3 vViewPosition;

void main() {



    vec2 screenUv = gl_FragCoord.xy / vec2(textureSize(gPosition, 0));

    vec4 worldPosf = texture(gPosition, screenUv).rgba;

    if (worldPosf.a <= 0.0) discard;

    vec3 normal = texture(gNormal, screenUv).rgb;
    vec3 worldPos = worldPosf.xyz;

    float dist = distance(worldPos, uLightPosition);

    if (dist >= uLightRadius) {
        discard;
    }

    float attenuation = max(0.0, 1.0 - dist / uLightRadius);
    float brightness = uLightIntensity * (attenuation * attenuation);

    vec3 lightDir = normalize(uLightPosition - worldPos);
    float dotBrightness = max(dot(normal, lightDir), 0.0);

    vec3 color = texture(gAlbedo, screenUv).rgb;
    float specIntensity = texture(gAlbedo, screenUv).a;

    // Specular
    vec3 viewDir = normalize(vViewPosition - worldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), specIntensity * 32.0);

    // Specular must share the distance falloff, otherwise it gets cut off in
    // a hard circle at the light radius by the discard above.
    // brightness already includes uLightIntensity -> don't multiply it again.
    vec3 final = color * uLightColor * brightness * (dotBrightness + spec);

    lightTexture = final;
    //lightTexture = vec3(uLightPosition) + vec3(1.0, 0.0, 0.0);
}