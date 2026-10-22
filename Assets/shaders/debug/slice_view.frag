#version 430 core

in vec2 uv;
out vec4 FragColor;

uniform sampler3D _Volume;
uniform float _SliceZ;          // normalized [0,1] — which slice to view
uniform int _Mode;              // 0 = raw R channel (volumeTex/signTex), 1 = seed position as RGB, 2 = validity (w) as grayscale
uniform float _DisplayScale;    // for distance values — divide by something to fit [0,1] for viewing

void main() {
    vec4 texel = texture(_Volume, vec3(uv, _SliceZ));

    if (_Mode == 0) {
        // raw distance/sign — remap roughly to [0,1] for visibility
        // negative (inside) = red tint, positive (outside) = blue tint
        float v = texel.r / _DisplayScale;
        FragColor = (v < 0.0) ? vec4(-v, 0, 0, 1) : vec4(0, 0, v, 1);
    }
    else if (_Mode == 1) {
        // rgb output
        FragColor = vec4(texel.xyz / _DisplayScale, 1.0);
    }
    else if (_Mode == 2) {
        // r channel as grayscale
        FragColor = vec4(vec3(texel.r) / _DisplayScale, 1.0);
    }
}