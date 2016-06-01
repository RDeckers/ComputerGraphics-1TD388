// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_normal;
in vec3 v_world_position;
in vec4 v_color_spec;

layout(location = 0) out vec4 o_albedo_specularity;
layout(location = 1) out vec3 o_position; //worldspace
layout(location = 2) out vec3 o_normal;

void main()
{
  o_albedo_specularity = v_color_spec;
  o_position = v_world_position;
  o_normal = normalize(v_normal);
}
