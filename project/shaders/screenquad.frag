// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require
#extension  	ARB_texture_gather : require
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
layout(location = 3) uniform sampler2D u_occlusion_buffer;

uniform vec2 screen_size = vec2(640);

//layout(location = 6) uniform vec3 u_eye_position;

uniform vec3 light_positions[128];
uniform vec3 light_colors[128];
uniform float light_radius[128];


in vec2 v_texCoord;

void main()
{
  vec3 normal = normalize(texture(u_normal_buffer, v_texCoord).xyz);
  vec3 position = texture(u_position_buffer, v_texCoord).xyz;
  vec3 color = texture(u_color_buffer, v_texCoord).rgb;
  float ambient_occlusion = 0.25*dot(vec4(1), textureGather(u_occlusion_buffer, v_texCoord, 0));
 //float ambient_occlusion = texture(u_occlusion_buffer, v_texCoord+0.5/screen_size).r; //use the linear filter to get rid off noise at the cost of half a fragment offset.
  float alpha = texture(u_color_buffer, v_texCoord).a;
  
  vec3 N = normalize(normal);
  vec3 V = normalize(-position);
  vec3 total_color = vec3(0);
  for(int i = 0; i < 1; i++){
    vec3 r = (light_positions[i]-position);
    vec3 L = normalize(r);
    vec3 halfway = normalize(L+V);
    float NL = dot(N,L);
    float diffuse = clamp(NL, 0, 1);
    float specular = NL > 0 ? ((alpha+8)/8)*pow(max(dot(N, halfway),0), alpha) : 0;
    float light_intensity = 1-smoothstep(0, light_radius[i], length(r));
    total_color += (diffuse+specular)*color*light_colors[i]*light_intensity;
   }
  o_frag_color = vec4(color*ambient_occlusion+total_color,1);
  //o_frag_color.a = 1;
}
