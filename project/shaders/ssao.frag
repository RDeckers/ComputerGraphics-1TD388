#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require
layout(location = 0) out float o_occlusion;

layout(location = 0) uniform sampler2D u_position_buffer;
layout(location = 1) uniform sampler2D u_normal_buffer;
layout(location = 2) uniform float u_radius = 0.05;//vase this off of view-space depth?
layout(location = 3) uniform int u_sample_count = 64;
layout(location = 4) uniform mat4 u_projection_matrix;
layout(location = 5) uniform vec3 u_rotations[4] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(0,0,0)};


uniform vec3 u_rays[128];

in vec2 v_texCoord;

float linear_depth(vec2 at){
  return texture(u_position_buffer, at).z;
}

void main(void)
{
  vec3 position = texture(u_position_buffer, v_texCoord).xyz;
  
  vec3 normal = normalize(2*texture(u_normal_buffer, v_texCoord).xyz-1);
  
  vec3 random_shift = u_rotations[int((mod(gl_FragCoord.x+0.5, 2)))+2*int(mod(gl_FragCoord.y+0.5, 2))];
  vec3 tangent = normalize(random_shift - normal * dot(random_shift, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 tbn = mat3(tangent, bitangent, normal);
  
  float depth = linear_depth(v_texCoord);
  float occlusion_count = 0;
  float radius = u_radius/1;//(1+(depth-0.1)/(100-0.1));
  for(int i = 0; i < u_sample_count; i++){
    vec3 ray = tbn*normalize(u_rays[i]);
    float scale = float(i) / float(u_sample_count);
    scale = mix(0.1f, 1.0f, scale * scale);
    //float scale = 1;
    ray *=  scale*radius;
    ray += position;
    // project sample position:
    vec4 sample_texel = vec4(ray, 1.0);
    sample_texel = u_projection_matrix * sample_texel;
    sample_texel.xy /= sample_texel.w;
    sample_texel.xy = sample_texel.xy * 0.5 + 0.5;
    vec3 sampled_position = texture(u_position_buffer, sample_texel.xy).xyz;
    
    //float rangeCheck = 1-smoothstep(0.0, 1.0, radius / length(position - sampled_position));
    float rangeCheck= abs(position.z - sampled_position.z) < u_radius ? 1.0 : 0.0;
    occlusion_count += (sampled_position.z > ray.z ? 1.0 : 0.0) * rangeCheck;    
  }
  o_occlusion = pow(1-occlusion_count/float(u_sample_count),3);
  //o_frag_color.rgb = abs(random_shift);//u_projection_matrix*vec4(position,1);
}

