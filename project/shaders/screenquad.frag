// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

/*
  TODO:
  Defered rendered:
    *position(depth only or possible full 3D so we compute "real distances" and light drop-off)
    *normals
    *diffuse color & specular power.
  compute occlusion using semispheres & random rotation.
   use distance checking.
  blur to (albedo?) buffer
  add lights. render to screen.
   scale radius with distance from eye in some way so that it's world-based?
   https://www.google.se/?client=firefox-b-ab#q=ssao+remove+banding&gfe_rd=cr
*/

layout(location = 0) out vec4 o_frag_color;

layout(location = 0) uniform sampler2D u_color_buffer;
layout(location = 1) uniform sampler2D u_normal_buffer;
layout(location = 2) uniform sampler2D u_position_buffer;

//layout(location = 1) uniform vec2 screen_size = vec2(640);

layout(location = 6) uniform vec3 u_eye_position;

uniform vec3 light_positions[128];
uniform vec3 light_colors[128];
uniform float light_radius[128];


in vec2 v_texCoord;

void main()
{
  vec3 normal = texture(u_normal_buffer, v_texCoord).xyz;
  vec3 position = texture(u_position_buffer, v_texCoord).xyz;
  vec3 color = texture(u_color_buffer, v_texCoord).rgb;
  float alpha = texture(u_color_buffer, v_texCoord).a;
  
  vec3 N = normalize(normal);
  vec3 V = normalize(u_eye_position - position);
  vec3 total_color = vec3(0);
  for(int i = 0; i < 32; i++){
    vec3 r = (light_positions[i]-position);
    vec3 L = normalize(r);
    vec3 halfway = normalize(L+V);
    float NL = dot(N,L);
    float diffuse = clamp(NL, 0, 1);
    float specular = NL > 0 ? ((alpha+8)/8)*pow(max(dot(N, halfway),0), alpha) : 0;
    float light_intensity = 1-smoothstep(0, light_radius[i], length(r));
    total_color += (diffuse+specular)*color*light_colors[i]*light_intensity;
   }
  o_frag_color = vec4(total_color+0.05*color,1);
  //o_frag_color.a = 1;
}
