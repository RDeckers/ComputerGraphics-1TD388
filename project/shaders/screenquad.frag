// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) out vec4 frag_color;

layout(location = 0) uniform sampler2D renderedTexture;
layout(location = 1) uniform vec2 screen_size = vec2(640);

layout(location = 2) uniform float sample_radius= 0.05;
layout(location = 3) uniform uint sample_count = 128;
layout(location = 4) uniform vec3 sample_directions[128];


in vec2 v_texCoord;

void main()
{
  uint occlusion_count = 0;
  float depth = texture(renderedTexture, v_texCoord).r;
  for(uint u =0; u < sample_count; u++){
    float sampled_depth = texture(
          renderedTexture,
          v_texCoord+sample_radius*sample_directions[u].xy
    ).r;
    if(depth +sample_directions[u].z <=  sampled_depth)
      occlusion_count += 1;
  }
//  vec3 mean = 0.2*(c+l+r+u+d);
//  vec3 deviation = 128*abs(c-mean);
  //float grayscale = (texel.r+texel.g+texel.b)/3.0;
  frag_color = vec4(vec3(occlusion_count)/sample_count,1);
}
