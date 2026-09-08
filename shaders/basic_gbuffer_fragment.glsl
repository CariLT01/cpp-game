#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUv;

uniform sampler2D uAlbedoSpecTexture;
uniform sampler2D uNormalTexture;

// Computes TBN matrix using screen-space derivatives of Position and UVs
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv) {
    // Get screen-space derivatives of surface position and texture coordinates
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    // Solve linear system for tangent vectors
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // Normalize vectors
    float invmax = inversesqrt(max(dot(T,T), dot(B,B)));
    return mat3(T * invmax, B * invmax, N);
}

void main() {
    gAlbedoSpec = texture(uAlbedoSpecTexture, vUv).rgba;
    gPosition = vec4(vWorldPos, 1.0);

    mat3 TBN = cotangent_frame(normalize(vNormal), vWorldPos, vUv);

    vec3 mapNormal = texture(uNormalTexture, vUv).rgb * 2.0 - 1.0;
    vec3 finalNormal = normalize(TBN * mapNormal);

    gNormal = vec4(finalNormal, 1.0);
}