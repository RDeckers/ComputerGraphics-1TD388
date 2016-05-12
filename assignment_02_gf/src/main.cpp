#include <gameframe.hpp>
#include <GL/glew.h>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL2/SDL.h>
#include <glu.h>
#include <utilities/logging.h>
#include <utilities/file.h>
#include <cube.hpp>
#include <fixatedcamera.hpp>
#include <object.hpp>

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
  GLuint vao, vbo;
  GLuint program;
  Cube *cube;
  Object *object;
  FixatedCamera *camera;
  double last_gametime = 0;
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
    glClearColor(0.4,0.6,0.8,1.0);
    glEnable(GL_DEPTH_TEST);

    program = progamFromFilenames("../shaders/pos_col_norm_3d.vert", "../shaders/col_norm.frag");
    object = new Object("../objects/gargo.obj");
    //cube = new Cube();
    last_gametime = SDL_GetTicks();
    camera = new FixatedCamera(glm::vec3(0,0,0), 0, 0, 3);
    /*camera.place(glm::vec3(0,0.5,1));
    camera.look_at(glm::vec3(0,0,0));*/
  }

  void draw(double gametime_ms){
    glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->update();
    object->draw();
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
    //cube->update(gametime_ms, 0);
    return 0;
  }

  void renderer_cleanup(){
    delete object;
    //delete cube;
  }
};

int main(int argc, char** argv){
  set_cwdir_to_bin_dir();
  Game my_game;
  my_game.run();
  SDL_Quit();
  return 0;
}
