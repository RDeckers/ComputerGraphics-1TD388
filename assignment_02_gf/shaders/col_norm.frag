// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 2) uniform vec3 u_diffuse_color;
layout(location = 3) uniform vec3 u_eye_position;

in vec3 v_normal;
in vec3 v_position;

layout(location = 0) out vec4 frag_color;

vec3 ambient_color = vec3(1);
vec3 specular_color = vec3(1);

vec3 light_pos = vec3(0,0,10);
float Ka = 0.1;
float Kd = 0.5;
float Ks = 1.0;
float alpha = 16;

void main()
{
  //light_pos = u_eye_position;
  vec3 light_dir = normalize(light_pos-v_position);
  vec3 view_dir = normalize(u_eye_position - v_position);
  
  vec3 halfway = normalize(light_dir+view_dir);
  float diffuse = Kd*max(dot(v_normal, light_dir),0);
  float specular = Ks*pow(max(dot(v_normal, halfway),0), alpha);
  frag_color = vec4(Ka*ambient_color+diffuse*u_diffuse_color+specular*specular_color,1);
}
