// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require
#extension  	ARB_texture_gather : require

layout(location = 0) out vec4 o_frag_color;

layout(location = 0) uniform sampler2D u_color_buffer;
layout(location = 1) uniform sampler2D u_normal_buffer;
layout(location = 2) uniform sampler2D u_position_buffer;
layout(location = 3) uniform sampler2D u_occlusion_buffer;
layout(location = 4) uniform int u_light_count = 0;
layout(location = 5) uniform float u_bias = 0;
layout(location = 6) uniform mat4 u_view_matrix;

uniform vec2 screen_size = vec2(640);

uniform vec3 light_positions[128];
uniform vec3 light_colors[128];
uniform float light_radius[128];


in vec2 v_texCoord;

void main()
{
  vec3 normal = normalize(texture(u_normal_buffer, v_texCoord).xyz);
  vec3 position = texture(u_position_buffer, v_texCoord).xyz;//view space
  vec3 color = texture(u_color_buffer, v_texCoord).rgb;
  float ambient_occlusion = 0.25*dot(vec4(1), textureGather(u_occlusion_buffer, v_texCoord, 0));
  float alpha = texture(u_color_buffer, v_texCoord).a;
  
  vec3 N = normalize(normal);
  vec3 V = normalize(position);//to eye, eye = 0.
  vec3 total_color = vec3(0);
  for(int i = 0; i < u_light_count; i++){
    vec3 light_pos = (u_view_matrix*vec4(light_positions[i],1)).xyz; /*light fixed in worldspace*/
    vec3 r = (light_pos - position);
    vec3 L = normalize(r);
    vec3 halfway = normalize(L+V);
    float NL = dot(N,L);
    float diffuse = clamp(NL, 0, 1);
    float specular = NL > 0 ? ((alpha+8)/8)*pow(max(dot(N, halfway),0), alpha) : 0;
    float light_intensity = 1-smoothstep(0, light_radius[i], length(r));
    total_color += (diffuse+specular)*color*light_intensity*light_colors[i];
   }
  o_frag_color = vec4(total_color + color*min(1,u_bias+ambient_occlusion),1);
  //o_frag_color.a = 1;
}
