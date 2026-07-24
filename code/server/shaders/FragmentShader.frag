#version 460

layout(location = 0) out vec3 color;

void main() {
  color = vec4(color, 0);
}
