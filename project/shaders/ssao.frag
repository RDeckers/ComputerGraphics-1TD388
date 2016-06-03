#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) out vec4 o_frag_color;

layout(location = 0) uniform sampler2D u_position_buffer;
layout(location = 1) uniform sampler2D u_normal_buffer;
layout(location = 2) uniform float u_radius = 0.01;//vase this off of view-space depth?
layout(location = 3) uniform int u_sample_count = 64;

layout(location = 6) uniform vec3 u_rays[128];

in vec2 v_texCoord;

float linear_depth(vec2 at){
  return texture(u_position_buffer, at).w;
}

void main(void)
{
  vec3 position = texture(u_position_buffer, v_texCoord).xyz;
  float depth = linear_depth(v_texCoord);
  float occlusion_count = 0;
  float radius = u_radius/1;//(1+(depth-0.1)/(100-0.1));
  for(int i = 0; i < u_sample_count; i++){
    vec3 ray = normalize(u_rays[i]);
    float scale = float(i) / float(u_sample_count);
    scale = mix(0.1f, 1.0f, scale * scale);
    ray *=  scale*radius;
    vec2 sample_coord = v_texCoord +  ray.xy;
    float sampled_depth = linear_depth(sample_coord);
    vec3 sampled_position = texture(u_position_buffer, sample_coord).xyz;
    float rangeCheck = 1-smoothstep(0.0, 1.0, radius / length(position - sampled_position));
    occlusion_count += (sampled_depth < depth+ray.z ? 1.0 : 0.0) * rangeCheck;    
  }
  o_frag_color = vec4(vec3(pow(1-occlusion_count/float(u_sample_count),1)), 1.0);
}

