#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

class Camera
{
  float m_fov = 45;
  float m_aspect_ratio = 1.0f;
  float m_near_clipping = 0.1f;
  float m_far_clipping = 100.0f;
  glm::mat4 m_projection_matrix;
  
  void update_projection_matrix(){
    m_projection_matrix =glm::perspective(m_fov, m_aspect_ratio, m_near_clipping, m_far_clipping);
  }

  glm::mat4 view_matrix(){
    return glm::lookAt(get_position(), get_gaze(), get_up());
  }
  
  glm::mat4 projection_matrix(){
    return m_projection_matrix;
  }

  glm::mat4 pv_matrix(){
    return projection_matrix()*view_matrix();
  }

public:
  Camera();
  ~Camera();

  virtual glm::vec3 get_position() =0;
  virtual glm::vec3 get_gaze() =0;
  virtual glm::vec3 get_up(){return glm::vec3(0,1,0);}
  void bind_projection_matrix(GLuint binding){
    glUniformMatrix4fv(binding, 1, GL_FALSE, glm::value_ptr(this->projection_matrix()));
  }
  void bind_view_matrix(GLuint binding){
    glUniformMatrix4fv(binding, 1, GL_FALSE, glm::value_ptr(this->view_matrix()));
  }

  void update(){
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(this->view_matrix()));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(this->projection_matrix()));
  }
};

#endif // CAMERA_H
