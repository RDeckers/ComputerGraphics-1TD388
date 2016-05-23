#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Object
{
  enum AttributeLocation {
    POSITION = 0,
    NORMAL = 2,
  };
  GLuint m_vbo[2] = {0};
  GLuint m_vao = 0;
  std::vector<GLuint> m_indices;
  std::vector<glm::vec3> m_vertices;
  std::vector<glm::vec3> m_normals;
  
  glm::vec3 m_scale = glm::vec3(1.0f);
  glm::vec3 m_translation = glm::vec3(0.0f);
  glm::quat m_rotation;
  
  glm::mat4 model_matrix();
  void compute_normals();
  void populate_gl_buffers();
public:
  Object(const char* obj_file);
  ~Object();
  bool load_file(const char *obj_file);
  void draw();
  
};

#endif // OBJECT_HPP
