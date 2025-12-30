#version 400
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform float _Degrees;

void main()
{
    vec3 color = texture(_MainTex, uv).rgb;

    float r = color.r;
    float g = color.g;
    float b = color.b;

    float cmin = min(r, min(g, b));
    float cmax = max(r, max(g, b));
    float diff = cmax - cmin;

    float h = 0.0;
    if (diff == 0.0) {
        FragColor = vec4(color, 1.0);
        return;
    }
    else if (cmax == r) h = (g - b) / diff;
    else if (cmax == g) h = (b - r) / diff + 2.0;
    else                h = (r - g) / diff + 4.0;

    h = mod(h + _Degrees / 60.0, 6.0);

    float v = cmax;
    float s = (v == 0.0) ? 0.0 : diff / v;

    float c = v * s;
    float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
    float m = v - c;

    vec3 rgb;
    if      (h < 1.0) rgb = vec3(c, x, 0);
    else if (h < 2.0) rgb = vec3(x, c, 0);
    else if (h < 3.0) rgb = vec3(0, c, x);
    else if (h < 4.0) rgb = vec3(0, x, c);
    else if (h < 5.0) rgb = vec3(x, 0, c);
    else              rgb = vec3(c, 0, x);

    FragColor = vec4(rgb + m, 1.0);
}
