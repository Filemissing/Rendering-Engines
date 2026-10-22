#version 400 core

in vec2 uv;
out vec4 FragColor;

// -- Volume bounds & slice info --
uniform vec3 _WorldMin;
uniform vec3 _WorldMax;
uniform int  _SliceZ;
uniform int  _ResolutionZ;

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
    vec3  halfExtents;
    vec3  color;
};

uniform Primitive primitives[MAX_PRIMITIVES];
uniform int primitiveCount;

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
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_BOX) {
            d = BoxSDF(localP, vec3(0.0), primitives[i].halfExtents);
            d *= min(primitives[i].scale.x,
            min(primitives[i].scale.y, primitives[i].scale.z));
        }
        else if (primitives[i].type == TYPE_PYRAMID) {
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
        else {
            d = -999; // no-primitive type matched
        }

        if (d < h.dist) {
            h.dist = d;
            h.id   = i; // use index directly — look up color from array
        }
    }

    return h;
}

void main() {
//    FragColor = vec4(primitives[0].rotation, 1);
//    return;

    // (x, y) comes from the quad's uv, matching this slice's resolution
    // z is reconstructed from which slice we're currently baking
    vec3 uvw = vec3(uv.x, uv.y, (float(_SliceZ) + 0.5) / float(_ResolutionZ));
    vec3 worldPos = _WorldMin + uvw * (_WorldMax - _WorldMin);

    Hit h = mapScene(worldPos);

    FragColor = vec4(h.dist, 0.0, 0.0, 1.0);
}