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

//layout(location = 2) uniform float sample_radius= 0.05;
//layout(location = 3) uniform uint sample_count = 128;
//layout(location = 4) uniform vec3 sample_directions[128];


in vec2 v_texCoord;

void main()
{
//  float occlusion_count = 0;
//  float depth = texture(renderedTexture, v_texCoord).r;
//  for(uint u =0; u < sample_count; u++){
//    vec3 offset = normalize(sample_directions[u]);
//    offset *= sample_radius;
//    float sampled_depth = texture(
//          renderedTexture,
//          v_texCoord+offset.xy
//    ).r;
//    occlusion_count += (sampled_depth < depth + offset.z? 1.0 : 0.0); 
//  }
  vec3 normal = texture(u_position_buffer, v_texCoord).xyz;
  o_frag_color = vec4(0.5*(1-normal),1);
}
