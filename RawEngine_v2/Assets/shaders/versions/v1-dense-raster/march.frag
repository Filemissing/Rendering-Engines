// Assets/shaders/PostProcessing/raymarching_volume.frag
#version 400 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D _MainTex;
uniform sampler2D _SceneDepth;
uniform sampler3D _VolumeTex;

uniform vec3 _WorldMin;
uniform vec3 _WorldMax;

uniform mat4 _InvView;
uniform mat4 _InvProj;
uniform vec3 _CameraPos;
uniform float _Far;

uniform int _MaxSteps;
uniform float _MaxDist;
uniform float _SurfDist;

float sceneRayDist() {
    float depth = texture(_SceneDepth, uv).r;
    if (depth >= 0.9999) return _Far;
    vec3 ndc = vec3(uv * 2.0 - 1.0, depth * 2.0 - 1.0);
    vec4 viewPos = _InvProj * vec4(ndc, 1.0);
    viewPos /= viewPos.w;
    vec4 worldPos = _InvView * viewPos;
    return length(worldPos.xyz - _CameraPos);
}

vec3 rayDirection() {
    vec2 ndc = uv * 2.0 - 1.0;
    vec4 viewPos = _InvProj * vec4(ndc, -1.0, 1.0);
    viewPos.xyz /= viewPos.w;
    viewPos.w = 0.0;
    return normalize((_InvView * viewPos).xyz);
}

// -- volume sampling --
vec3 worldToUVW(vec3 p) {
    return (p - _WorldMin) / (_WorldMax - _WorldMin);
}

float sampleVolume(vec3 p) {
    return texture(_VolumeTex, worldToUVW(p)).r;
}

vec3 calcNormal(vec3 p) {
    const float e = 0.5; // in world units — matches ~1 voxel; tune per volume size
    return normalize(vec3(
    sampleVolume(p + vec3(e,0,0)) - sampleVolume(p - vec3(e,0,0)),
    sampleVolume(p + vec3(0,e,0)) - sampleVolume(p - vec3(0,e,0)),
    sampleVolume(p + vec3(0,0,e)) - sampleVolume(p - vec3(0,0,e))
    ));
}

vec3 shade(vec3 pos, vec3 normal) {
    vec3 lightDir = normalize(vec3(0.6, 1.0, 0.4));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 col = vec3(1.0); // no color available from the volume yet
    return col * (0.15 + 0.85 * diff);
}

void main() {
    vec3 ro = _CameraPos;
    vec3 rd = rayDirection();

    float sceneDepthLinear = sceneRayDist();

    float t = 0.0;
    bool hit = false;

    for (int i = 0; i < _MaxSteps; i++) {
        vec3 p = ro + rd * t;
        float d = sampleVolume(p);

        if (d < _SurfDist) {
            hit = true;
            break;
        }

        t += d;

        if (t >= sceneDepthLinear || t >= _MaxDist) break;
    }

    if (hit) {
        vec3 pos = ro + rd * t;
        vec3 normal = calcNormal(pos);
        vec3 color = shade(pos, normal);
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = texture(_MainTex, uv);
    }
}