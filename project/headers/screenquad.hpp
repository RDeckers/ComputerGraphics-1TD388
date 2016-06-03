#ifndef SCREENQUAD_H
#define SCREENQUAD_H
#include <gl/glew.h>

class ScreenQuad
{
  const static GLfloat vertices[];
  GLuint m_vbo = 0;
  GLuint m_vao = 0;
  GLuint m_framebuffer = 0;
  
  GLuint m_depthbuffer;
  
  GLuint m_position_texture = 0;
  GLuint m_normal_texture = 0;
  GLuint m_color_spec_texture = 0;
  
  GLuint m_width, m_height;
public:
  ScreenQuad(GLuint width = 128, GLuint height = 128);
  ~ScreenQuad();
  void bind_depth_texture(GLuint bindinding);
  void bind_normal_texture(GLuint bindinding);
  void bind_color_texture(GLuint binding);
  void bind();
  void draw();
};

#endif // SCREENQUAD_H
