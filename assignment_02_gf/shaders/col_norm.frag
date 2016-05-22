// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require


layout(location = 2) uniform vec3 u_eye_position;

layout(location = 3) uniform vec3 u_diffuse_color;
layout(location = 4) uniform vec3 u_ambient_color;
layout(location = 5) uniform vec3 u_specular_color;

layout(location = 6) uniform float u_gamma;
layout(location = 7) uniform float u_display_normals;
layout(location = 8) uniform float u_invert_colors;

layout(location = 9) uniform samplerCube u_cubemap;

in vec3 v_normal;
in vec3 v_world_position;

layout(location = 0) out vec4 frag_color;

vec3 ambient_color = vec3(0.1);
vec3 specular_color = vec3(1);

//vec3 light_pos = vec3(0,0,3);
//float Ka = 1 ;
//float Kd = 1;
//float Ks = 1;
float alpha = 16;

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
  frag_color.rgb = u_display_normals*(0.5+0.5*N)+(1-u_display_normals)*frag_color.rgb;
  vec3 R = normalize(reflect(-V, N));
  frag_color.rgb = texture(u_cubemap, R).rgb;
  frag_color.rgb = abs(pow(frag_color.rgb, vec3(1 / u_gamma))-u_invert_colors); //todo: should be inverse
}
