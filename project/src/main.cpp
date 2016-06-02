#include <gameframe.hpp>
#include <GL/glew.h>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL2/SDL.h>
#include <glu.h>
#include <utilities/logging.h>
#include <utilities/file.h>
#include <utilities/rng.h>
#include <cube.hpp>
#include <fixatedcamera.hpp>
#include <object.hpp>
#include <screenquad.hpp>
#include <glm/glm.hpp>

GLuint progamFromFilenames(const char* vertexShaderFile, const char *fragmentShaderFile){
  char *shader_log = NULL;
  GLuint vertex_shader =  gluShaderFromFile(vertexShaderFile, GL_VERTEX_SHADER);
  if(!gluShaderIsCompiled(vertex_shader)){
    report(FAIL, "failed to compile %s", vertexShaderFile);
  }
  if(gluGetShaderLog(vertex_shader, GLU_DYNAMIC, &shader_log)){
    report(WARN, "%s log:\n%s", vertexShaderFile, shader_log);
  }

  GLuint fragment_shader =  gluShaderFromFile(fragmentShaderFile, GL_FRAGMENT_SHADER);
  if(!gluShaderIsCompiled(fragment_shader)){
    report(FAIL, "failed to compile %s", fragmentShaderFile);
  }
  if(gluGetShaderLog(fragment_shader, GLU_DYNAMIC, &shader_log)){
    report(WARN, "%s log:\n%s", fragmentShaderFile, shader_log);
  }

  GLuint shaderProgram= glCreateProgram();
  glAttachShader(shaderProgram, fragment_shader);
  glAttachShader(shaderProgram, vertex_shader);
  glLinkProgram(shaderProgram);
  return shaderProgram;
}

class Game : public Gameframe{
  enum AttributeLocation {
    POSITION = 0,
    COLOR = 1,
    NORMAL = 2,
  };
  GLfloat samples[3*128];
  GLuint program;
  GLuint screenquad_program;
  ScreenQuad *screen;
  Cube *cube;
  Object *object;
  FixatedCamera *camera;
  double last_gametime = 0;
  
  void create_samples(unsigned count){
    for(unsigned u = 0; u < count; u++){
      samples[3*u+0] = fast_rngf(-1, 1);
      samples[3*u+1] = fast_rngf(-1, 1);
      samples[3*u+2] = fast_rngf(-1, 1);
    }
  }
  
public:
  Game(){}
  ~Game(){}

protected:
  void initialize(){
    this->fps_limit = 5000;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    if(-1 == SDL_GL_SetSwapInterval(-1)){
      report(INFO,"Late tearing not supported!");
      if(-1== SDL_GL_SetSwapInterval(1)){
        report(WARN,"VSync not supported?!");
      }
    }
    glClearColor(0,0,0,0);
    glEnable(GL_DEPTH_TEST);

    program = progamFromFilenames("../shaders/object_instance_to_G.vert", "../shaders/object_to_G.frag");
    screenquad_program = progamFromFilenames("../shaders/screenquad_passthrough.vert", "../shaders/screenquad.frag");
    glUseProgram(program);
    screen = new ScreenQuad(get_width(), get_height());
    object = new Object("../objects/teapot.obj");
    last_gametime = SDL_GetTicks();
    camera = new FixatedCamera(glm::vec3(0,0,0), 0, 0, 2);
    glUseProgram(screenquad_program);
    create_samples(128);
    glUniform3fv(4, 128, samples);
  }

  void draw(double gametime_ms){
    glUseProgram(program);
    screen->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->update();
    object->draw();
    
    glUseProgram(screenquad_program);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,get_width(),get_height()); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screen->draw();
  }

  int update(double gametime_ms){
    double tick_time = last_gametime - gametime_ms;
    last_gametime = gametime_ms;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_A]) {
        camera->rotate_yaw((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_D]) {
        camera->rotate_yaw(-(float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_W]) {
      camera->rotate_pitch((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_S]) {
      camera->rotate_pitch(-(float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_Q]) {
      camera->move_distance((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_E]) {
        camera->move_distance(-(float)tick_time*0.003f);
    }
    return 0;
  }

  void renderer_cleanup(){
    delete object;
  }
};

int main(int argc, char** argv){
  set_cwdir_to_bin_dir();
  Game my_game;
  my_game.run();
  SDL_Quit();
  return 0;
}
