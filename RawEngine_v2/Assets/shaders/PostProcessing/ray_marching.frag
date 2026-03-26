// Assets/shaders/PostProcessing/raymarching.frag
#version 330 core

in vec2 uv;
out vec4 FragColor;

// -- Standard pipeline uniforms --
uniform sampler2D _MainTex;
uniform sampler2D _SceneColor;
uniform sampler2D _SceneDepth;
uniform vec4 _TexelSize;

// -- Camera uniforms --
uniform mat4 _InvView;
uniform mat4 _InvProj;
uniform vec3 cameraPos;
uniform float _Near;
uniform float _Far;

// -- Objects --
#define MAX_PRIMITIVES 32
#define TYPE_SPHERE 0
#define TYPE_BOX    1

struct Primitive {
    int   type;
    vec3  position;
    vec3  rotation;
    vec3  scale;
    vec3  halfExtents; // box only — sphere uses halfExtents.x as radius
    vec3  color;
};

uniform Primitive primitives[MAX_PRIMITIVES];
uniform int primitiveCount;

// ----------------------------------------------------------------
// Depth reconstruction
// ----------------------------------------------------------------
float linearizeDepth(float d) {
    float z = d * 2.0 - 1.0; // NDC
    return (2.0 * _Near * _Far) / (_Far + _Near - z * (_Far - _Near));
}

float sceneLinearDepth() {
    float d = texture(_SceneDepth, uv).r;
    if (d >= 0.9999) return _Far; // sky / nothing
    return linearizeDepth(d);
}

// Reconstruct world-space ray for this fragment
vec3 rayDirection() {
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 viewPos = _InvProj * vec4(ndc, -1.0, 1.0);
    viewPos.xyz /= viewPos.w;
    viewPos.w = 0.0;

    vec3 worldDir = normalize((_InvView * viewPos).xyz);
    return worldDir;
}

// ----------------------------------------------------------------
// SDF primitives
// ----------------------------------------------------------------
float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float sdBox(vec3 p, vec3 center, vec3 halfExtents) {
    vec3 q = abs(p - center) - halfExtents;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// ----------------------------------------------------------------
// Scene definition — edit this to place your primitives
// ----------------------------------------------------------------
struct Hit {
    float dist;
    int   id;   // which object was hit (for colouring)
};

mat3 eulerToMat3(vec3 euler) {
    float cx = cos(euler.x), sx = sin(euler.x);
    float cy = cos(euler.y), sy = sin(euler.y);
    float cz = cos(euler.z), sz = sin(euler.z);

    // ZYX order — most common convention (matches GLM default)
    mat3 rx = mat3(
    1,   0,   0,
    0,  cx, -sx,
    0,  sx,  cx
    );
    mat3 ry = mat3(
    cy,  0,  sy,
    0,  1,   0,
    -sy,  0,  cy
    );
    mat3 rz = mat3(
    cz, -sz,  0,
    sz,  cz,  0,
    0,   0,  1
    );

    return rz * ry * rx;
}

Hit mapScene(vec3 p) {
    Hit h;
    h.dist = 1e9;
    h.id   = -1;

    for (int i = 0; i < primitiveCount; i++) {
        float d;
        vec3 localP = eulerToMat3(primitives[i].rotation) * (p - primitives[i].position);
        localP /= primitives[i].scale; // apply scale

        if (primitives[i].type == TYPE_SPHERE) {
            d = sdSphere(localP, vec3(0.0), primitives[i].halfExtents.x);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z)); // correct scale
        }
        else if (primitives[i].type == TYPE_BOX) {
            d = sdBox(localP, vec3(0.0), primitives[i].halfExtents);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }

        if (d < h.dist) {
            h.dist = d;
            h.id   = i; // use index directly — look up color from array
        }
    }

    return h;
}

// ----------------------------------------------------------------
// Helper methods for reconstructing object info
// ----------------------------------------------------------------
vec3 calcNormal(vec3 p) {
    const float e = 0.001;
    return normalize(vec3(
    mapScene(p + vec3(e,0,0)).dist - mapScene(p - vec3(e,0,0)).dist,
    mapScene(p + vec3(0,e,0)).dist - mapScene(p - vec3(0,e,0)).dist,
    mapScene(p + vec3(0,0,e)).dist - mapScene(p - vec3(0,0,e)).dist
    ));
}



// ----------------------------------------------------------------
// Simple diffuse + ambient shading
// ----------------------------------------------------------------
vec3 shade(vec3 pos, vec3 normal, int id) {
    vec3 lightDir = normalize(vec3(0.6, 1.0, 0.4));
    float diff    = max(dot(normal, lightDir), 0.0);
    vec3  col     = primitives[id].color;
    return col * (0.15 + 0.85 * diff);
}

// ----------------------------------------------------------------
// Raymarcher
// ----------------------------------------------------------------
uniform int _MaxSteps;
uniform float _MaxDist;
uniform float _SurfDist;

void main() {
    vec3 ro = cameraPos;
    vec3 rd = rayDirection();

    float sceneDepthLinear = sceneLinearDepth();

    float t    = 0.0;
    int   hitId = -1;
    int steps = 0;

    for (int i = 0; i < _MaxSteps; i++) {
        vec3  p = ro + rd * t;
        Hit   h = mapScene(p);

        if (h.dist < _SurfDist) {
            hitId = h.id;
            steps = i;
            break;
        }

        t += h.dist;

        // Stop if we've passed existing geometry or marched too far
        if (t >= sceneDepthLinear || t >= _MaxDist) {
            steps = i;
            break;
        }
    }

//    FragColor = vec4(primitiveCount, 0, 0, 1);
//    return;

    if (hitId != -1) {
        vec3 pos    = ro + rd * t;
        vec3 normal = calcNormal(pos);
        vec3 color  = shade(pos, normal, hitId);
        FragColor   = vec4(color, 1.0);
    } else {
        // No hit - pass through the existing scene
        FragColor = texture(_MainTex, uv);
    }
}