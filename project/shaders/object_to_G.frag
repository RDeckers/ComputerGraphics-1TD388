// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_normal;
in vec3 v_position;
in vec4 v_color_spec;

layout(location = 0) out vec4 o_albedo_specularity;
layout(location = 1) out vec3 o_normal;
layout(location = 2) out vec4 o_position; //worldspace

uniform float far_plane = 100;
uniform float near_plane = 0.1;

float linear_depth(float depth){
  float z = depth * 2.0 - 1.0; // Back to NDC 
  return (2.0 * near_plane * far_plane) / (far_plane+ near_plane- z * (far_plane- near_plane));	
}

void main()
{
  o_albedo_specularity = v_color_spec;
  o_position.rgb = v_position;
  o_position.a = linear_depth(gl_FragCoord.z);
  o_normal = normalize(v_normal);
}
