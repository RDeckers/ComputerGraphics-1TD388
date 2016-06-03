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
  
  // The depth buffer
  glGenRenderbuffers(1, &m_depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);
  
  glActiveTexture(GL_TEXTURE0 + 0);
  glGenTextures(1, &m_color_spec_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenTextures: %s (%d)", glewGetErrorString(error), error);
  glBindTexture(GL_TEXTURE_2D, m_color_spec_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindTexture: %s (%d)", glewGetErrorString(error), error);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, m_width, m_height, 0,GL_RGBA, GL_FLOAT, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexImage2D: %s (%d)", glewGetErrorString(error), error);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexParameteri: %s (%d)", glewGetErrorString(error), error);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_color_spec_texture, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glFramebufferTexture: %s (%d)", glewGetErrorString(error), error);
  
  glActiveTexture(GL_TEXTURE0 + 1);
  glGenTextures(1, &m_normal_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenTextures: %s (%d)", glewGetErrorString(error), error);
  glBindTexture(GL_TEXTURE_2D, m_normal_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindTexture: %s (%d)", glewGetErrorString(error), error);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, m_width, m_height, 0,GL_RGB, GL_FLOAT, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexImage2D: %s (%d)", glewGetErrorString(error), error);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexParameteri: %s (%d)", glewGetErrorString(error), error);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_normal_texture, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glFramebufferTexture: %s (%d)", glewGetErrorString(error), error);
  
  glActiveTexture(GL_TEXTURE0 + 2);
  glGenTextures(1, &m_position_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glGenTextures: %s (%d)", glewGetErrorString(error), error);
  glBindTexture(GL_TEXTURE_2D, m_position_texture);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glBindTexture: %s (%d)", glewGetErrorString(error), error);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, m_width, m_height, 0,GL_RGBA, GL_FLOAT, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexImage2D: %s (%d)", glewGetErrorString(error), error);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glTexParameteri: %s (%d)", glewGetErrorString(error), error);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_position_texture, 0);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glFramebufferTexture: %s (%d)", glewGetErrorString(error), error);
  
//  glActiveTexture(GL_TEXTURE0 + 3);
//  glGenTextures(1, &m_depth_texture);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_texture, 0);
  
  GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, DrawBuffers);
  if(GL_NO_ERROR != (error= glGetError()))
    report(FAIL, "glDrawBuffers: %s (%d)", glewGetErrorString(error), error);
}

ScreenQuad::~ScreenQuad()
{
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
  glDeleteTextures(1, &m_color_spec_texture);
  glDeleteTextures(1, &m_position_texture);
  glDeleteTextures(1, &m_normal_texture);
  glDeleteRenderbuffers(1, &m_depthbuffer);
  glDeleteFramebuffers(1, &m_framebuffer);
}

void ScreenQuad::bind(){
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glViewport(0,0,m_width,m_height); 
}

void ScreenQuad::bind_depth_texture(GLuint binding){
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, m_position_texture);
  glUniform1i(binding, 2);
}

void ScreenQuad::draw(){
  glBindVertexArray(m_vao);
//  glUniform1i(0, 0);
//  glActiveTexture(GL_TEXTURE0 + 0);
//  glBindTexture(GL_TEXTURE_2D, m_color_spec_texture);
//  glUniform1i(1, 1);
//  glActiveTexture(GL_TEXTURE0 + 1);
//  glBindTexture(GL_TEXTURE_2D, m_normal_texture);
//  glUniform1i(2, 2);
//  glActiveTexture(GL_TEXTURE0 + 2);
//  glBindTexture(GL_TEXTURE_2D, m_position_texture);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
