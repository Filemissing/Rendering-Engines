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

bool intersectAABB(vec3 ro, vec3 rd, out float tEnter, out float tExit) {
    vec3 invD = 1.0 / rd;
    vec3 t0 = (_WorldMin - ro) * invD;
    vec3 t1 = (_WorldMax - ro) * invD;
    vec3 tMin = min(t0, t1);
    vec3 tMax = max(t0, t1);
    tEnter = max(max(tMin.x, tMin.y), tMin.z);
    tExit  = min(min(tMax.x, tMax.y), tMax.z);
    return tExit >= max(tEnter, 0.0);
}

void main() {
    vec3 ro = _CameraPos;
    vec3 rd = rayDirection();

    float sceneDepthLinear = sceneRayDist();

    float tEnter, tExit;
    if (!intersectAABB(ro, rd, tEnter, tExit)) {
        FragColor = texture(_MainTex, uv); // Ray never touches the box
        return;
    }

    // Start at the box wall (or 0.0 if the camera is inside the box)
    float t = max(0.0, tEnter);

    // Stop marching if we exit the box, hit the far plane, or hit scene geometry
    float tMax = min(min(tExit, _MaxDist), sceneDepthLinear);

    bool hit = false;

    for (int i = 0; i < _MaxSteps; i++) {
        vec3 p = ro + rd * t;
        float d = sampleVolume(p);

        if (d < _SurfDist) {
            hit = true;
            break;
        }

        t += d;

        if (t >= tMax) break;
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