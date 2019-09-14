#version 440
layout(binding = 0, offset = 0) uniform atomic_uint counter;
layout(rgba32f, binding = 0) uniform writeonly image2D posTex;
layout(rgba8, binding = 1) uniform writeonly image2D colorTex;
uniform sampler2DRect colorImage;
uniform float lumThres;

layout(std430, binding = 0) buffer vertices{
    float position[];
};

layout(std430, binding = 1) buffer texCoords{
    float texcoord[];
};

vec3 sobelFilter(in sampler2DRect img, in vec2 uv) {

    const vec2 offset[9] = vec2[](
        vec2(-1.0, -1.0), vec2( 0.0, -1.0), vec2( 1.0, -1.0),
        vec2(-1.0,  0.0), vec2( 0.0,  0.0), vec2( 1.0,  0.0),
        vec2(-1.0,  1.0), vec2( 0.0,  1.0), vec2( 1.0,  1.0)
    );

    const float hCoef[9] = float[](
        1.0,  0.0, -1.0,
        2.0,  0.0, -2.0,
        1.0,  0.0, -1.0
    );

    const float vCoef[9] = float[](
        1.0,  2.0,  1.0,
        0.0,  0.0,  0.0,
        -1.0, -2.0, -1.0
    );

    vec3 horizonColor = vec3(0.0);
    vec3 verticalColor = vec3(0.0);

    for (int i = 0; i < 9; i++) {
        vec3 c = texture(img, uv + offset[i]).rgb;
        horizonColor += c * hCoef[i];
        verticalColor += c * vCoef[i];
    }
    return vec3(sqrt(horizonColor * horizonColor + verticalColor * verticalColor));
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {

    vec2 size = textureSize(colorImage);
    int index = int(gl_GlobalInvocationID.x);

    vec3 pos = vec3(position[index * 3], position[index * 3 + 1], position[index * 3 + 2]);
    if (all(equal(pos, vec3(0)))) return;

    // Let's filter depth buffer
    vec2 uv = vec2(texcoord[index * 2], texcoord[index * 2 + 1]) * size;

    // Apply Sobel filter
    vec3 color = sobelFilter(colorImage, uv);
    //vec3 color = texture(colorImage, uv).rgb;

    float depth = - pos.z;
    float lum = dot(vec3(1.), color) / 3.;
    float depthLimit = 2.0;

    if (lum > lumThres && depth < depthLimit) {

        // Atomic Operation!
        int current = int(atomicCounterIncrement(counter));

        int x = int(mod(float(current), size.x));
        int y = int(floor(current / size.x));

        imageStore(posTex, ivec2(x, y), vec4(pos, depth / depthLimit));
        imageStore(colorTex, ivec2(x, y), vec4(texture(colorImage, uv).rgb, 1.0));

    }
}
