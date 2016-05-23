#include <screenquad.hpp>
#include <utilities/logging.h>

const GLfloat ScreenQuad::vertices[] = {
  -1.0, -1.0,
  +1.0, -1.0,
  -1.0, +1.0,
  +1.0, +1.0
};

ScreenQuad::ScreenQuad(GLuint width, GLuint height)
{
  GLenum error;
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "Something went wrong before setting up the framebuffer: %s (%d)", glewGetErrorString(error), error);
  report(INFO, "making a %ux%u screenquad", width, height);
  m_width = width;
  m_height = height;
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenVertexArrays", glewGetErrorString(error), error);
  glGenBuffers(1, &m_vbo);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenBuffersr: %s (%d)", glewGetErrorString(error), error);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindBuffer: %s (%d)", glewGetErrorString(error), error);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBufferData: %s (%d)", glewGetErrorString(error), error);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glVertexAttribPointer: %s (%d)", glewGetErrorString(error), error);
  glEnableVertexAttribArray(0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glEnableVertexAttribArray: %s (%d)", glewGetErrorString(error), error);
  glGenFramebuffers(1, &m_framebuffer);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenFramebuffers: %s (%d)", glewGetErrorString(error), error);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindFramebuffer: %s (%d)", glewGetErrorString(error), error);
  glGenTextures(1, &m_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenTextures: %s (%d)", glewGetErrorString(error), error);
  
  glBindTexture(GL_TEXTURE_2D, m_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindTexture: %s (%d)", glewGetErrorString(error), error);
  
  glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, m_width, m_height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexImage2D: %s (%d)", glewGetErrorString(error), error);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexParameteri: %s (%d)", glewGetErrorString(error), error);
  
  // The depth buffer
  glGenRenderbuffers(1, &m_depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);
  
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glFramebufferTexture: %s (%d)", glewGetErrorString(error), error);
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glDrawBuffers: %s (%d)", glewGetErrorString(error), error);
}

ScreenQuad::~ScreenQuad()
{
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
  glDeleteTextures(1, &m_texture);
  glDeleteFramebuffers(1, &m_framebuffer);
}

void ScreenQuad::bind(){
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glViewport(0,0,m_width,m_height); 
}

void ScreenQuad::draw(){
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
