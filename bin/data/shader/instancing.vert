#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
in vec4 position; // oF Default
in vec2 texcoord; // oF Default

uniform sampler2DRect posTex;
uniform sampler2DRect colorTex;
uniform float voxelSize;

uniform float lds;
uniform float time;

out vec3 vColor;
out vec4 vPos;
out float vDepth;

void main() {
    // Instanced rendering
    vec2 size = textureSize(colorTex);
    ivec2 uv = ivec2(
        mod(float(gl_InstanceID), size.x),
        floor(float(gl_InstanceID) / size.x)
    );

    vec4 s = texelFetch(posTex, uv);
    vec3 p = s.xyz * 1000.;

    //float factor = ((clamp(sin(-time * 4. + p.y * 0.03), -1.0, -0.8) + 0.8) * 2.0 + 1.0);
    float factor = 1.;

    p += position.xyz * voxelSize * s.a * factor;
    p.z -= factor * 10.;

    vec3 color = pow(texelFetch(colorTex, uv).rgb, vec3(0.5));

    vPos = modelViewMatrix * vec4(p, 1.);
    vDepth = - vPos.z * lds;
    vColor = mix(mix(color, vec3(0), s.a), vec3(0.9, 0.9, 1.2), 1. - factor);
    gl_Position = modelViewProjectionMatrix * vec4(p, 1.);
}
