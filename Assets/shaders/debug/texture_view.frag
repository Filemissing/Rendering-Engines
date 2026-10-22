#version 430 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D _Texture;
uniform int _Mode;              // 0 = raw R channel (volumeTex/signTex), 1 = seed position as RGB, 2 = validity (w) as grayscale
uniform float _DisplayScale;    // for distance values — divide by something to fit [0,1] for viewing

void main() {
    vec4 pixel = texture(_Texture, uv);

    if (_Mode == 0) {
        // raw distance/sign — remap roughly to [0,1] for visibility
        // negative (inside) = red tint, positive (outside) = blue tint
        float v = pixel.r / _DisplayScale;
        FragColor = (v < 0.0) ? vec4(-v, 0, 0, 1) : vec4(0, 0, v, 1);
    }
    else if (_Mode == 1) {
        // rgb output
        FragColor = vec4(pixel.rgb / _DisplayScale, 1.0);
    }
    else if (_Mode == 2) {
        // r channel as grayscale
        FragColor = vec4(vec3(pixel.r) / _DisplayScale, 1.0);
    }
}