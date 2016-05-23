#ifndef FIXATEDCAMERA_H
#define FIXATEDCAMERA_H
#include <camera.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#define _USE_MATH_DEFINES
#include <cmath> 
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class FixatedCamera : public Camera
{
  //TODO: use a quaternion?
  glm::float32 m_distance, m_pitch, m_yaw;
  glm::vec3 m_fixation_point;
public:
  FixatedCamera(glm::vec3 fixation_point, glm::float32 pitch, glm::float32 yaw, glm::float32 distance);
  ~FixatedCamera();
  glm::vec3 get_position();
  glm::vec3 get_gaze();
  glm::vec3 get_up();
  void rotate_yaw(float amount);
  void rotate_pitch(float amount);
  void move_distance(float amount);
  
};

#endif // FIXATEDCAMERA_H
