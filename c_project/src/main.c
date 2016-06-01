#define NO_STDIO_REDIRECT
#include <gbuffer.h>
#include <object_loader.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glu.h>
#include <utilities/logging.h>
#include <utilities/file.h>
#include <utilities/rng.h>

int REPORT_W_COLORS = 1;
int SHOULD_EXIT = 0;

int main(int argc, char** argv){
  GLenum error;
  //First we need to start up SDL, and make sure it went ok
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    report(FAIL, "SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  set_cwdir_to_bin_dir();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  if (SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)){
    report(FAIL, "SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL\
  /*| SDL_WINDOW_BORDERLESS*/); //can also se to be fullscreen-desktop, borderless, input grabbed or high DPI if supported.
  if (win == NULL){
    report(FAIL, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GLContext glcontext = SDL_GL_CreateContext(win);
  if (glcontext == NULL){
    report(FAIL, "SDL_GLContext Error: %s\n", SDL_GetError());
    return -1;
  }
  if(GL_NO_ERROR != (error= glGetError())){
    report(FAIL, "%s %u: %s (%d)", __func__, __LINE__,glewGetErrorString(error), error);
  }
  //glewExperimental = GL_TRUE;
  glewInit();
  glGetError(); //glew sets an error.
  if(GL_NO_ERROR != (error= glGetError())){
    report(FAIL, "%s %u: %s (%d)", __func__, __LINE__,glewGetErrorString(error), error);
  }

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

  Gbuffer_t *gbuffer = Gbuffer_new(640, 480);
  object_t object;
  object_init(&object);
  object_load_file(&object, "../resources/objects/bunny.obj");
  object_compute_normals(&object);
  uint32_t tick, minimum_ticks = 10; //10ms = 100 Hz
  do{
    tick = SDL_GetTicks();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(win);

    uint32_t elapsed_ticks = SDL_GetTicks()-tick;
    if(elapsed_ticks < minimum_ticks){
      uint32_t sleep = minimum_ticks-elapsed_ticks;
      SDL_Delay(sleep);
    }
	}while (!(SHOULD_EXIT=SDL_QuitRequested())); //pumps events and stops if a quit was issued.
  //Gbuffer_free(gbuffer);
  SDL_Quit();
}
