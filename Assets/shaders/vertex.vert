#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv;

uniform mat4 mvpMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fPos;
out vec4 wPos;
out vec3 fNor;
out vec4 wNor;
out vec2 uv;

void main() {
  fPos = aPos;
  wPos = modelMatrix * vec4(aPos, 1);
  fNor = aNor;
  wNor = modelMatrix * vec4(aNor, 0);
  uv = aUv;
  gl_Position = mvpMatrix * vec4(aPos, 1.0);
}