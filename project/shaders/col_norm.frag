// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require


layout(location = 2) uniform vec3 u_eye_position;

layout(location = 3) uniform vec3 u_diffuse_color =vec3(0.8, 0.2, 0.25);
layout(location = 4) uniform vec3 u_ambient_color =  vec3(0, 0.1,0.1);
layout(location = 5) uniform vec3 u_specular_color = 0.1*vec3(1,1,1);

in vec3 v_normal;
in vec3 v_world_position;

layout(location = 0) out vec4 frag_color;

float alpha = 12;

void main()
{
  vec3 light_pos = u_eye_position;

  vec3 N = normalize(v_normal);
  vec3 L = normalize(light_pos-v_world_position); //L
  vec3 V = normalize(u_eye_position - v_world_position); //V

  vec3 halfway = normalize(L+V);
  float NL = dot(N, L);
  float diffuse = clamp(NL,0,1);
  float specular = NL > 0 ? ((alpha+8)/8)*pow(max(dot(N, halfway),0), alpha) : 0;
  frag_color = vec4(u_ambient_color+diffuse*u_diffuse_color+specular*u_specular_color,1);
}
