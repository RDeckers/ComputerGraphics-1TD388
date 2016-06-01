#include <gbuffer.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <utilities/logging.h>

Gbuffer_t *Gbuffer_new(unsigned width, unsigned height){
  Gbuffer_t *buffer = malloc(sizeof(Gbuffer_t));
  if(!buffer){
    report(FAIL, "Could not allocate Gbuffer_t: %s (%d)", strerror(errno), errno);
    return NULL;
  }

  buffer->width = width;
  buffer->height = height;

  glGenFramebuffers(1, &buffer->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);

  // The depth buffer
  glGenRenderbuffers(1, &buffer->depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, buffer->depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->depthbuffer);

  glActiveTexture(GL_TEXTURE0 + 0);
  glGenTextures(1, &buffer->color_spec_texture);
  glBindTexture(GL_TEXTURE_2D, buffer->color_spec_texture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, width, height, 0,GL_RGBA, GL_FLOAT, 0);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer->color_spec_texture, 0);

  glActiveTexture(GL_TEXTURE0 + 1);
  glGenTextures(1, &buffer->position_texture);
  glBindTexture(GL_TEXTURE_2D, buffer->position_texture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, width, height, 0,GL_RGB, GL_FLOAT, 0);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, buffer->position_texture, 0);

  glActiveTexture(GL_TEXTURE0 + 2);
  glGenTextures(1, &buffer->normal_texture);
  glBindTexture(GL_TEXTURE_2D, buffer->normal_texture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, width, height, 0,GL_RGB, GL_FLOAT, 0);
  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, buffer->normal_texture, 0);

  GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0+1, GL_COLOR_ATTACHMENT0+2};
  glDrawBuffers(3, DrawBuffers);
  return buffer;
}

void Gbuffer_free(Gbuffer_t *buffer){
  glDeleteTextures(1, &buffer->color_spec_texture);
  glDeleteTextures(1, &buffer->position_texture);
  glDeleteTextures(1, &buffer->normal_texture);
  glDeleteRenderbuffers(1, &buffer->depthbuffer);
  glDeleteFramebuffers(1, &buffer->framebuffer);
  free(buffer);
}

void Gbuffer_bind(Gbuffer_t *buffer){
  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
  glViewport(0,0,buffer->width,buffer->height);
}
