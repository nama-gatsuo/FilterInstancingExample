#version 400
in vec3 vColor;
in vec4 vPos;
in float vDepth;

uniform int isShadow;
out vec4 outputColor0;
out vec4 outputColor1;
out vec4 outputColor2;

vec3 calcFlatNormal(vec3 posInViewSpace){
    vec3 dx = dFdx(posInViewSpace);
    vec3 dy = dFdy(posInViewSpace);
    vec3 n = normalize(cross(normalize(dx), normalize(dy)));

    return - n;
}

void main(){
    if (isShadow == 1) {
        outputColor0.r = vDepth;
        outputColor0.a = 1.0;
    } else {
        outputColor0 = vec4(vColor, 1.);
        outputColor1 = vPos;
        outputColor2 = vec4(calcFlatNormal(vPos.xyz), vDepth);
    }
}
