// Assets/shaders/PostProcessing/raymarching.frag
#version 330 core

in vec2 uv;
out vec4 FragColor;

// -- Standard pipeline uniforms --
uniform sampler2D _MainTex;
uniform sampler2D _SceneDepth;

// -- Camera uniforms --
uniform mat4 _InvView;
uniform mat4 _InvProj;
uniform vec3 cameraPos;
uniform float _Far;

// -- Objects --
#define MAX_PRIMITIVES 128
#define TYPE_SPHERE    0
#define TYPE_BOX       1
#define TYPE_PYRAMID   2
#define TYPE_CAPSULE   3
#define TYPE_CYLINDER  4
#define TYPE_TORUS     5
#define TYPE_CONE      6
#define TYPE_PLANE     7

struct Primitive {
    int   type;
    vec3  position;
    vec3  rotation;
    vec3  scale;
    vec3  halfExtents; // box only 
    vec3  color;
};

uniform Primitive primitives[MAX_PRIMITIVES];
uniform int primitiveCount;

// Reconstruct distance from depth
float sceneRayDist() {
    float depth = texture(_SceneDepth, uv).r;
    if (depth >= 0.9999) return _Far;

    // Reconstruct NDC position
    vec3 ndc = vec3(uv * 2.0 - 1.0, depth * 2.0 - 1.0);

    // Unproject to view space
    vec4 viewPos = _InvProj * vec4(ndc, 1.0);
    viewPos /= viewPos.w;

    // Transform to world space
    vec4 worldPos = _InvView * viewPos;

    // True distance from camera to that world position along the ray
    return length(worldPos.xyz - cameraPos);
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
float SphereSDF(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float BoxSDF(vec3 p, vec3 center, vec3 halfExtents) {
    vec3 q = abs(p - center) - halfExtents;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float PyramidSDF(vec3 p, float height, float baseHalfSize) {
    // Fold into the first quadrant — pyramid is 4-fold symmetric
    p.xz = abs(p.xz);

    // Slope of the face
    float slope = baseHalfSize / height;

    // Distance to the slanted face
    float face = p.y * slope + p.x + p.z - baseHalfSize;
    if (p.x < p.z) face = p.y * slope + p.z + p.x - baseHalfSize;

    // Clamp to pyramid region
    float d = max(face * inversesqrt(slope * slope + 2.0),
    -p.y);              // bottom cap
    d = max(d, p.y - height);        // top cap

    return d;
}
// Capsule — halfExtents.x = radius, halfExtents.y = half length (along Y axis)
float CapsuleSDF(vec3 p, float radius, float halfLen) {
    p.y -= clamp(p.y, -halfLen, halfLen);
    return length(p) - radius;
}

// Cylinder — halfExtents.x = radius, halfExtents.y = half height
float CylinderSDF(vec3 p, float radius, float halfHeight) {
    vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(radius, halfHeight);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

// Torus — halfExtents.x = major radius (ring), halfExtents.y = minor radius (tube)
float TorusSDF(vec3 p, float major, float minor) {
    vec2 q = vec2(length(p.xz) - major, p.y);
    return length(q) - minor;
}

// Cone — halfExtents.x = base radius, halfExtents.y = height (apex at +Y, base at -Y)
float ConeSDF(vec3 p, float radius, float height) {
    p.y -= height * 0.5;
    vec2 q = vec2(length(p.xz), -p.y);
    vec2 tip = vec2(radius, height);
    vec2 a = q - tip * clamp(dot(q, tip) / dot(tip, tip), 0.0, 1.0);
    vec2 b = q - tip * vec2(clamp(q.x / tip.x, 0.0, 1.0), 1.0);
    float s = sign(tip.y * q.x - tip.x * q.y);
    float d = min(dot(a, a), dot(b, b));
    return sqrt(d) * sign(max(s, -sign(p.y + height * 0.5)));
}

// Infinite plane — halfExtents.xyz = normal, halfExtents.w would be offset
// Since we only have vec3, pass normal in halfExtents and offset in scale.x
float PlaneSDF(vec3 p, vec3 normal, float offset) {
    return dot(p, normalize(normal)) - offset;
}

// ----------------------------------------------------------------
// Scene definition
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
            d = SphereSDF(localP, vec3(0.0), primitives[i].halfExtents.x);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z)); // correct scale
        }
        else if (primitives[i].type == TYPE_BOX) {
            d = BoxSDF(localP, vec3(0.0), primitives[i].halfExtents);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_PYRAMID){
            d = PyramidSDF(localP, primitives[i].halfExtents.y, primitives[i].halfExtents.x);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_CAPSULE) {
            d = CapsuleSDF(localP,
            primitives[i].halfExtents.x,
            primitives[i].halfExtents.y);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_CYLINDER) {
            d = CylinderSDF(localP,
            primitives[i].halfExtents.x,
            primitives[i].halfExtents.y);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_TORUS) {
            d = TorusSDF(localP,
            primitives[i].halfExtents.x,
            primitives[i].halfExtents.y);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_CONE) {
            d = ConeSDF(localP,
            primitives[i].halfExtents.x,
            primitives[i].halfExtents.y);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_PLANE) {
            // Plane ignores scale correction — it's infinite
            d = PlaneSDF(localP,
            primitives[i].halfExtents,
            primitives[i].scale.x);
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

    float sceneDepthLinear = sceneRayDist();

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