#include <object_loader.h>
#include <utilities/logging.h>
#include <utilities/file.h>
#include <math.h>

void object_init(object_t *object){
  dynarrGLf_init(&object->vertices, 0);
  dynarrGLf_init(&object->normals, 0);
  dynarrGLu_init(&object->indices, 0);
}

void object_load_file(object_t *object, const char* filename){
  report(INFO, "before loading num vertices: %u", object->vertices.count);
  FILE *handle = fopen (filename, "r");
  if(!handle){
    report(FAIL, "Couldn't open file %s: %s (%d)", filename, strerror(errno), errno);
    return;
  }
  ssize_t line_length;
  char* line = NULL;
  unsigned line_count = 0;
  size_t buffer_size = 0;
  while(-1 != (line_length = getline(&line, &buffer_size, handle))){
    line_count++;
    if(0 == strncmp("v ", line, 2)){//starts with "v "
      GLfloat vertices[3];
      if(-1 == sscanf(line, "v %f %f %f", vertices+0, vertices+1, vertices+2)){
        report(WARN, "Malformed input in %s:%u \"%s\"", filename, line_count, line);
        continue;
      }
      dynarrGLf_push_array(&object->vertices, vertices, 3);
    }else if(0 == strncmp("n ", line, 2)){
      GLfloat vertices[3];
      if(-1 == sscanf(line, "n %f %f %f", vertices+0, vertices+1, vertices+2)){
        report(WARN, "Malformed input in %s:%u \"%s\"", filename, line_count, line);
        continue;
      }
      dynarrGLf_push_array(&object->vertices, vertices, 3);
    }else if(0 == strncmp("f ", line, 2)){//face line
      GLuint indices[3];
      if(-1 == sscanf(line, "f %u %u %u", indices+0, indices+1, indices+2)){
        report(WARN, "Malformed input in %s:%u \"%s\"", filename, line_count, line);
        continue;
      }
      dynarrGLu_push_array(&object->indices, indices, 3);
    }
    else if('#' != line[0]){
      line[line_length-1] = 0;
      report(WARN, "Unrecognized input in object file %s:%u \"%s\"", filename, line_count, line);
    }
  }
  free(line);
  fclose(handle);
  report(INFO, "after loading num vertices: %u", object->vertices.count);
}

int object_compute_normals(object_t *object){
  if(-1 == dynarrGLf_resize(&object->normals, object->vertices.count)){
    report(FAIL, "failed to resize normals to proper size");
    return -1;
  }
  report(PASS, "resized to %u", object->vertices.size);
  GLfloat *vertices = object->vertices.base;
  GLfloat *normals = object->normals.base;
  unsigned num_indices = object->indices.count;
  GLuint *indices = object->indices.base;
  for(unsigned u = 0; u < object->normals.size; u+=3){
    normals[u+0] = 0;
    normals[u+1] = 0;
    normals[u+2] = 0;
  }
  report(PASS, "zero'd all normals, looping %u indices", num_indices);
  for (unsigned i = 0; i < num_indices; i += 3) {
    GLuint vertexIndex0 = indices[i + 0];
    GLuint vertexIndex1 = indices[i + 1];
    GLuint vertexIndex2 = indices[i + 2];

    GLfloat u1 = vertices[vertexIndex1*3+0] - vertices[vertexIndex0*3+0];
    GLfloat u2 = vertices[vertexIndex1*3+1] - vertices[vertexIndex0*3+1];
    GLfloat u3 = vertices[vertexIndex1*3+2] - vertices[vertexIndex0*3+2];

    GLfloat v1 = vertices[vertexIndex2*3+0] - vertices[vertexIndex0*3+0];
    GLfloat v2 = vertices[vertexIndex2*3+1] - vertices[vertexIndex0*3+1];
    GLfloat v3 = vertices[vertexIndex2*3+2] - vertices[vertexIndex0*3+2];

    GLfloat n1 = u2*v3-u3*v2;
    GLfloat n2 = u3*v1-u1*v3;
    GLfloat n3 = u1*v2-u2*v1;

    normals[vertexIndex0*3+0] += n1;
    normals[vertexIndex0*3+1] += n2;
    normals[vertexIndex0*3+2] += n3;

    normals[vertexIndex1*3+0] += n1;
    normals[vertexIndex1*3+1] += n2;
    normals[vertexIndex1*3+2] += n3;

    normals[vertexIndex2*3+0] += n1;
    normals[vertexIndex2*3+1] += n2;
    normals[vertexIndex2*3+2] += n3;
  }
  for(unsigned u = 0; u < object->normals.size; u+=3){
    GLfloat n1 = normals[u+0];
    GLfloat n2 = normals[u+1];
    GLfloat n3 = normals[u+2];
    GLfloat len = sqrt(n1*n1+n2*n2+n3*n3);
    normals[u+0] = n1/len;
    normals[u+1] = n2/len;
    normals[u+2] = n3/len;
  }
  report(PASS, "Computed normals");
  return 0;
}
