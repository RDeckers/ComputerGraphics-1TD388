// Fragment shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_color;
in vec3 v_normal;

layout(location = 0) out vec4 frag_color;

void main()
{
  frag_color = vec4((0.5+0.5*v_normal),1);
}
