#ifndef _G_BUFFER_H
#define _G_BUFFER_H
#include <GL/glew.h>
typedef struct Gbuffer{
  GLuint framebuffer;

  GLuint depthbuffer;

  GLuint position_texture;
  GLuint normal_texture;
  GLuint color_spec_texture;

  GLuint width, height;
}Gbuffer_t;

Gbuffer_t *Gbuffer_new(unsigned width, unsigned height);
void Gbuffer_free(Gbuffer_t *buffer);
void Gbuffer_bind(Gbuffer_t *buffer);

#endif
