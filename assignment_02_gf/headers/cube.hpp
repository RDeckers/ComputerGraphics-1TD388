#ifndef CUBE_H
#define CUBE_H
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>

class Cube
{
  enum AttributeLocation {
    POSITION = 0,
    COLOR = 1,
    NORMAL = 2,
  };
  GLuint m_vao, m_vbo;
  glm::vec3 m_scale = glm::vec3(0.1f);
  glm::vec3 m_translation = glm::vec3(0.0f);
  glm::quat m_rotation;

  volatile bool m_has_changed = false;

  glm::mat4 model_matrix();
public:

  Cube();//TODO: share buffers between multiple invocations of Cube.
  ~Cube();
  void draw();
  void update(double global_time, double tick_time);
  void rotate(glm::vec3 axis, float angle){
    m_rotation *= glm::angleAxis(angle, axis);
    m_has_changed = true;
  }
  void set_rotation(glm::vec3 axis, float angle){
    m_rotation = glm::angleAxis(angle, axis);
    m_has_changed = true;
  }

  void scale(glm::vec3 scaling_factor);
  void set_scale(glm::vec3 scale);
  void translate(glm::vec3 translation);
  void set_translation(glm::vec3 translation);
};

#endif // CUBE_H
