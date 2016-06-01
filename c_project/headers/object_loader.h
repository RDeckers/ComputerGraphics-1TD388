#ifndef _OBJECT_LOADER_H
#define _OBJECT_LOADER_H

#include <types.h>

typedef struct object{
  dynarrGLf_t vertices;
  dynarrGLu_t indices;
  dynarrGLf_t normals;
}object_t;

void object_init(object_t *object);
void object_load_file(object_t *object, const char* filename);
int object_compute_normals(object_t *object);
object_t* object_new_from_file();
void object_free(object_t *object);

#endif
