// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform mat4 u_view_matrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color_spec;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in mat4 a_object_matrix;

out vec3 v_normal;
out vec3 v_world_position;
out vec4 v_color_spec;

void main()
{
  vec4 world_pos = a_object_matrix*vec4(a_position,1);
  gl_Position = u_view_matrix*world_pos;
  v_world_position = world_pos.xyz;
  v_normal= mat3(a_object_matrix)*a_normal;
  v_color_spec = a_color_spec;
}
