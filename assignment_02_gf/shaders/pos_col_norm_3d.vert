// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform mat4 object_matrix;
layout(location = 1) uniform mat4 view_matrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_color;

void main()
{
  gl_Position = view_matrix*object_matrix*vec4(a_position,1);
  v_color = a_color;
  v_normal= a_normal;
}
