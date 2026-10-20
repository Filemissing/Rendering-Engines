#version 400 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D _MainTex;
uniform sampler2D _SceneDepth;
uniform sampler3D _LookupTex;
uniform sampler3D _BrickPoolTex;

uniform vec3 _WorldMin;
uniform vec3 _WorldMax;

uniform vec3 _Resolution;
uniform vec3 _GridResolution;
uniform vec3 _PoolResolution;
uniform int BRICK_CORE;
uniform int BRICK_STORE;

uniform mat4 _InvView;
uniform mat4 _InvProj;
uniform vec3 _CameraPos;
uniform float _Far;

uniform int _MaxSteps;
uniform float _MaxDist;

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

float sampleVolumeRaw(vec3 p, float fallback) {
    vec3 voxel = (p - _WorldMin) / (_WorldMax - _WorldMin) * _Resolution;

    vec3 brickIndexF = floor(voxel / float(BRICK_CORE));
    brickIndexF = clamp(brickIndexF, vec3(0.0), _GridResolution - 1.0);
    ivec3 brickIndex = ivec3(brickIndexF);

    vec4 lookup = texelFetch(_LookupTex, brickIndex, 0);
    if (lookup.w < 0.5) return fallback; // no data here — assume "same as center", contributes ~0 to the gradient

    vec3 localOffset = clamp(voxel - brickIndexF * float(BRICK_CORE), 0.0, float(BRICK_STORE - 1));
    vec3 poolTexelCoord = lookup.xyz + localOffset;
    vec3 uvw = (poolTexelCoord + 0.5) / _PoolResolution;
    return texture(_BrickPoolTex, uvw).r;
}

vec3 calcNormal(vec3 p) {
    float center = sampleVolumeRaw(p, 0.0); // p itself is always inside an allocated brick — this hit, so fallback here is moot
    const float e = 0.5;
    return normalize(vec3(
    sampleVolumeRaw(p + vec3(e,0,0), center) - sampleVolumeRaw(p - vec3(e,0,0), center),
    sampleVolumeRaw(p + vec3(0,e,0), center) - sampleVolumeRaw(p - vec3(0,e,0), center),
    sampleVolumeRaw(p + vec3(0,0,e), center) - sampleVolumeRaw(p - vec3(0,0,e), center)
    ));
}

// -- analytic intersection --
// The ray is traced in voxel space, v(t) = vo + vd * t. That mapping is affine in the world-space t, so every
// t below is the world-space ray distance. Voxel sample i of brick b sits at integer voxel coordinate b*CORE + i,
// the same mapping sampleVolumeRaw uses.

// where the ray leaves the voxel-space box [bmin, bmax]
float boxExit(vec3 vo, vec3 invVd, vec3 bmin, vec3 bmax) {
    vec3 t0 = (bmin - vo) * invVd;
    vec3 t1 = (bmax - vo) * invVd;
    vec3 tFar = max(t0, t1);
    return min(min(tFar.x, tFar.y), tFar.z);
}

// cubic f(s) = k.w s^3 + k.z s^2 + k.y s + k.x
float evalCubic(vec4 k, float s) {
    return ((k.w * s + k.z) * s + k.y) * s + k.x;
}
float evalCubicDeriv(vec4 k, float s) {
    return (3.0 * k.w * s + 2.0 * k.z) * s + k.y;
}

// First zero crossing of the cubic on [0, h], given f(0) > 0. The derivative's roots (solved in closed form)
// split [0, h] into at most three monotone pieces. The first piece that ends at or below zero brackets the root,
// and a few safeguarded Newton steps refine it. A closed-form cubic solve is avoided on purpose: SDF cells are
// nearly linear, so k3 and k2 are usually ~0 and Cardano's formula loses most of its precision there.
bool firstRoot(vec4 k, float h, out float sHit) {
    sHit = 0.0;

    // roots of 3a s^2 + 2b s + c, using the cancellation-free quadratic form
    float qa = 3.0 * k.w, qb = 2.0 * k.z, qc = k.y;
    float e0 = h, e1 = h;
    float disc = qb * qb - 4.0 * qa * qc;
    if (disc >= 0.0) {
        float q = -0.5 * (qb + (qb >= 0.0 ? 1.0 : -1.0) * sqrt(disc));
        float r0 = (q != 0.0) ? qc / q : h;
        float r1 = (qa != 0.0) ? q / qa : h;
        r0 = (r0 > 0.0 && r0 < h) ? r0 : h;
        r1 = (r1 > 0.0 && r1 < h) ? r1 : h;
        e0 = min(r0, r1);
        e1 = max(r0, r1);
    }

    float lo = 0.0;
    float flo = k.x;
    for (int piece = 0; piece < 3; piece++) {
        float hi = (piece == 0) ? e0 : (piece == 1) ? e1 : h;
        if (hi <= lo) continue;
        float fhi = evalCubic(k, hi);

        if (fhi <= 0.0) {
            // monotone on [lo, hi] with flo > 0 >= fhi: start from the secant and refine
            float s = lo + flo / (flo - fhi) * (hi - lo);
            for (int i = 0; i < 4; i++) {
                float fs = evalCubic(k, s);
                if (fs > 0.0) lo = s; else hi = s;
                float df = evalCubicDeriv(k, s);
                float sNew = (df != 0.0) ? s - fs / df : 0.5 * (lo + hi);
                s = (sNew > lo && sNew < hi) ? sNew : 0.5 * (lo + hi);
            }
            sHit = s;
            return true;
        }

        lo = hi;
        flo = fhi;
    }
    return false;
}

// Intersect the ray with the trilinear interpolant of one voxel cell over [tc0, tc1].
bool intersectCell(vec3 vo, vec3 vd, ivec3 cell, ivec3 poolTexel, float tc0, float tc1, out float tHit) {
    tHit = tc0;

    float c000 = texelFetch(_BrickPoolTex, poolTexel + ivec3(0,0,0), 0).r;
    float c100 = texelFetch(_BrickPoolTex, poolTexel + ivec3(1,0,0), 0).r;
    float c010 = texelFetch(_BrickPoolTex, poolTexel + ivec3(0,1,0), 0).r;
    float c110 = texelFetch(_BrickPoolTex, poolTexel + ivec3(1,1,0), 0).r;
    float c001 = texelFetch(_BrickPoolTex, poolTexel + ivec3(0,0,1), 0).r;
    float c101 = texelFetch(_BrickPoolTex, poolTexel + ivec3(1,0,1), 0).r;
    float c011 = texelFetch(_BrickPoolTex, poolTexel + ivec3(0,1,1), 0).r;
    float c111 = texelFetch(_BrickPoolTex, poolTexel + ivec3(1,1,1), 0).r;

    // a trilinear interpolant is bounded by its corner values, so a cell whose corners share a sign has no surface
    float cMin = min(min(min(c000, c100), min(c010, c110)), min(min(c001, c101), min(c011, c111)));
    float cMax = max(max(max(c000, c100), max(c010, c110)), max(max(c001, c101), max(c011, c111)));
    if (cMin > 0.0 || cMax < 0.0) return false;

    // trilinear interpolant in monomial form: f = A + B.x x + B.y y + B.z z + Cxy xy + Cxz xz + Cyz yz + D xyz
    float A = c000;
    vec3 B = vec3(c100 - c000, c010 - c000, c001 - c000);
    float Cxy = c110 - c100 - c010 + c000;
    float Cxz = c101 - c100 - c001 + c000;
    float Cyz = c011 - c010 - c001 + c000;
    float D = c111 - c110 - c101 - c011 + c100 + c010 + c001 - c000;

    // substitute the ray (o + d s) in cell-local coordinates, with s = t - tc0, to get a cubic in s
    vec3 o = vo + vd * tc0 - vec3(cell);
    vec3 d = vd;

    vec4 k;
    k.w = D * d.x * d.y * d.z;
    k.z = Cxy * d.x * d.y + Cxz * d.x * d.z + Cyz * d.y * d.z
        + D * (o.x * d.y * d.z + o.y * d.x * d.z + o.z * d.x * d.y);
    k.y = dot(B, d)
        + Cxy * (o.x * d.y + o.y * d.x) + Cxz * (o.x * d.z + o.z * d.x) + Cyz * (o.y * d.z + o.z * d.y)
        + D * (o.x * o.y * d.z + o.x * o.z * d.y + o.y * o.z * d.x);
    k.x = A + dot(B, o) + Cxy * o.x * o.y + Cxz * o.x * o.z + Cyz * o.y * o.z + D * o.x * o.y * o.z;

    if (k.x <= 0.0) return true; // already inside at the cell entry (e.g. the camera or the volume edge is inside)

    float s;
    if (!firstRoot(k, tc1 - tc0, s)) return false;
    tHit = tc0 + s;
    return true;
}

// Walk the voxel cells of one active brick over [tStart, tEnd] (3D DDA) and solve each cell analytically.
bool traceBrick(vec3 vo, vec3 vd, vec3 invVd, float epsT, vec3 brickIndexF, ivec3 poolOrigin,
                float tStart, float tEnd, out float tHit) {
    tHit = tStart;

    ivec3 base = ivec3(brickIndexF) * BRICK_CORE;
    ivec3 cell = clamp(ivec3(floor(vo + vd * (tStart + epsT))), base, base + BRICK_CORE - 1);

    ivec3 stepDir = ivec3(sign(vd));
    vec3 tDelta = abs(invVd);
    vec3 tNext = (vec3(cell) + step(0.0, vd) - vo) * invVd; // where the ray crosses the cell's far face per axis

    float tc0 = tStart;
    for (int i = 0; i < 3 * BRICK_CORE; i++) { // a ray crosses at most 3*CORE-2 cells of a brick
        float tc1 = min(min(tNext.x, tNext.y), tNext.z);
        float segEnd = min(tc1, tEnd);

        if (segEnd > tc0 && intersectCell(vo, vd, cell, poolOrigin + (cell - base), tc0, segEnd, tHit))
            return true;
        if (tc1 >= tEnd) return false;

        if (tNext.x <= tNext.y && tNext.x <= tNext.z) { cell.x += stepDir.x; tNext.x += tDelta.x; }
        else if (tNext.y <= tNext.z)                  { cell.y += stepDir.y; tNext.y += tDelta.y; }
        else                                          { cell.z += stepDir.z; tNext.z += tDelta.z; }
        tc0 = tc1;

        if (any(lessThan(cell, base)) || any(greaterThanEqual(cell, base + BRICK_CORE))) return false;
    }
    return false;
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

    // ray in voxel space; zero direction components are nudged so the slab and DDA math never divides by zero
    vec3 voxelScale = _Resolution / (_WorldMax - _WorldMin);
    vec3 vo = (ro - _WorldMin) * voxelScale;
    vec3 vd = rd * voxelScale;
    vd = mix(vd, vec3(1e-8), lessThan(abs(vd), vec3(1e-8)));
    vec3 invVd = 1.0 / vd;
    float epsT = 1e-3 / max(max(abs(vd.x), abs(vd.y)), abs(vd.z)); // ~1/1000 of a voxel along the ray

    bool hit = false;

    // _MaxSteps counts brick visits; the cells inside a brick are bounded by traceBrick itself
    for (int i = 0; i < _MaxSteps; i++) {
        if (t >= tMax) break;

        vec3 brickIndexF = floor((vo + vd * (t + epsT)) / float(BRICK_CORE));
        brickIndexF = clamp(brickIndexF, vec3(0.0), _GridResolution - 1.0);

        float tBrickExit = boxExit(vo, invVd, brickIndexF * float(BRICK_CORE), (brickIndexF + 1.0) * float(BRICK_CORE));
        vec4 lookup = texelFetch(_LookupTex, ivec3(brickIndexF), 0);

        if (lookup.w <= -0.5) { hit = true; break; } // solid brick: the surface is at its face

        if (lookup.w >= 0.5) {
            float tHit;
            if (traceBrick(vo, vd, invVd, epsT, brickIndexF, ivec3(lookup.xyz), t, min(tBrickExit, tMax), tHit)) {
                t = tHit;
                hit = true;
                break;
            }
        }

        t = max(tBrickExit, t + epsT); // empty (or missed) brick — jump straight to its far face
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