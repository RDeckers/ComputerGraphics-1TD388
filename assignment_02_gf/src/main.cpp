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
#include <lodepng.h>


// Load cubemap texture and let OpenGL generate a mipmap chain
GLuint loadCubemap(const std::string &dirname)
{
    const char *filenames[] = { "posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png" };
    const GLenum targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    const unsigned num_sides = 6;

    std::vector<unsigned char> data[num_sides];
    unsigned width;
    unsigned height;
    for (unsigned i = 0; i < num_sides; ++i) {
        std::string filename = dirname + "/" + filenames[i];
        unsigned error = lodepng::decode(data[i], width, height, filename);
        if (error != 0) {
            //std::cout << "Error: " << lodepng_error_text(error) << std::endl;
            report(FAIL, "Error loading %s: %s", filename.c_str(), lodepng_error_text(error));
            std::exit(EXIT_FAILURE);
        }
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    for (unsigned i = 0; i < num_sides; ++i) {
        glTexImage2D(targets[i], 0, GL_SRGB8_ALPHA8, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, &(data[i][0]));
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

std::vector<GLuint> loadCubemaps(const std::string &dirname)
{
  std::vector<GLuint>  textures;
  const char* filters[] = {"0.125", "0.5", "2", "8", "32", "128", "512", "2048"};
  for(unsigned u = 0; u < 8; u++){
    textures.push_back(loadCubemap(dirname + "/" + "prefiltered/" + filters[u]));
  }
  return textures;
}


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
  std::vector<GLuint> cubemap;
  double last_gametime = 0;
  bool uniforms_changed = true;
  float gamma = 1.0;
  unsigned current_cubemap = 0;
  bool use_diffuse = false, use_ambient = true, use_specular = true, display_normals = false, invert_colors = false, use_gamma = true;
public:
  Game(){}
  ~Game(){}

  void load_uniforms(){
    //glUseProgram(program);
    use_diffuse? glUniform3f(3, 0.7, 0.1, 0.1) : glUniform3f(3, 0, 0, 0); //diffuse
    use_ambient? glUniform3f(4, 0.0, 0.1, 0.1) : glUniform3f(4, 0, 0, 0); //ambient
    use_specular? glUniform3f(5, 1, 1, 1) : glUniform3f(5, 0, 0, 0); //specular
    glUniform1f(6, use_gamma ? gamma : 1.0); //gamma
    glUniform1f(7, display_normals ? 1.0 : 0.0); //gamma
    glUniform1f(8, invert_colors ? 1.0 : 0.0); //gamma
    glUniform1i(9, 0);
  }

protected:
  void initialize(){
    this->fps_limit = 5000;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_CULL_FACE);
    if(-1 == SDL_GL_SetSwapInterval(-1)){
      report(INFO,"Late tearing not supported!");
      if(-1== SDL_GL_SetSwapInterval(1)){
        report(WARN,"VSync not supported?!");
      }
    }
    glClearColor(0.4,0.6,0.8,1.0);
    glEnable(GL_DEPTH_TEST);

    program = progamFromFilenames("../shaders/pos_col_norm_3d.vert", "../shaders/col_norm.frag");
    object = new Object("../objects/teapot.obj");
    //cube = new Cube();
    last_gametime = SDL_GetTicks();
    camera = new FixatedCamera(glm::vec3(0,0,0), 0, 0, 3);
    /*camera.place(glm::vec3(0,0.5,1));
    camera.look_at(glm::vec3(0,0,0));*/

    cubemap = loadCubemaps("../cubemaps/Forrest");

  }

  void draw(double gametime_ms){
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap[current_cubemap]);
    //if(uniforms_changed){
      load_uniforms();
      //uniforms_changed = false;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->update();
    object->draw();
  }

  int update(double gametime_ms){
    double tick_time = gametime_ms-last_gametime;
    last_gametime = gametime_ms;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_A]) {
        camera->rotate_yaw(-(float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_D]) {
        camera->rotate_yaw((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_W]) {
      camera->rotate_pitch(-(float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_S]) {
      camera->rotate_pitch((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_Q]) {
      camera->move_distance(-(float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_E]) {
        camera->move_distance((float)tick_time*0.003f);
    }
    if (state[SDL_SCANCODE_UP]) {
        gamma += (float)tick_time*0.003f;
        //report(INFO, "gamma = %f", gamma);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        gamma = (gamma > (float)tick_time*0.003f) ? gamma - (float)tick_time*0.003f : 0;
        //report(INFO, "gamma = %f", gamma);
    }
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      switch(event.type){
        // look for a keypress
        case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_1){
          use_ambient = !use_ambient;
        }
        if(event.key.keysym.sym == SDLK_2){
          use_diffuse = !use_diffuse;
        }
        if(event.key.keysym.sym == SDLK_3){
          use_specular = !use_specular;
        }
        if(event.key.keysym.sym == SDLK_g){
          use_gamma = !use_gamma;
        }
        if(event.key.keysym.sym == SDLK_n){
          display_normals = !display_normals;
        }
        if(event.key.keysym.sym == SDLK_i){
          invert_colors = !invert_colors;
        }
        if(event.key.keysym.sym == SDLK_PAGEDOWN){
          current_cubemap = current_cubemap == 0 ? 7:current_cubemap-1;
        }
        if(event.key.keysym.sym == SDLK_PAGEUP){
          current_cubemap = (current_cubemap+1) % 8;
        }
      }
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
