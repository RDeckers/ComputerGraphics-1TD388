#include <fixatedcamera.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>

FixatedCamera::FixatedCamera(glm::vec3 fixation_point, glm::float32 pitch, glm::float32 yaw, glm::float32 distance){
  m_fixation_point = fixation_point;
  m_pitch = pitch;
  m_yaw = yaw;
  m_distance = distance;
}

FixatedCamera::~FixatedCamera(){}

glm::vec3 FixatedCamera::get_position(){
  glm::vec3 base =glm::vec3(glm::yawPitchRoll(m_yaw, m_pitch, 0.0f)*glm::vec4(0.0f,0.0f,1.0f,1.0f));
  //glm::vec3 base = glm::vec3(glm::rotateX(glm::rotateY(glm::vec4(0.0f,0.0f,1.0f,1.0f), m_yaw), m_pitch));
  return m_distance*base+m_fixation_point;
}

glm::vec3 FixatedCamera::get_gaze(){
  return m_fixation_point;
}

void FixatedCamera::rotate_yaw(float amount){
  //amount = (1-2*(fabs(m_pitch) > M_PI/2))*amount;
  m_yaw += amount;
  while(fabs(m_yaw) > M_PI){
    m_yaw -= copysign(2*M_PI, amount);
  }
}
void FixatedCamera::rotate_pitch(float amount){
  m_pitch += amount;
  if(fabs(m_pitch) > M_PI/2){
    m_pitch = copysign(M_PI/2, amount);
  }
}

void FixatedCamera::move_distance(float amount){
  m_distance += amount;
}

glm::vec3  FixatedCamera::get_up(){
   return glm::vec3(glm::yawPitchRoll(m_yaw, m_pitch, 0.0f)*glm::vec4(0.0f,1.0f,0.0f,1.0f));
}
