#version 460

layout(location = 0) in vec2 positon;
layout(location = 1) in vec3 color;

void main() {
  gl_Position = vec4(positon.x, positon.y, 0, 0);
}
