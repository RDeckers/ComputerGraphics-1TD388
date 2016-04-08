// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

uniform float u_time;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

void main()
{
  v_color = a_color;
  vec2 offset = 0.3*vec2(cos(6.28f*u_time), sin(6.28f*u_time));
  gl_Position = vec4(a_position+offset,0,1);
}
