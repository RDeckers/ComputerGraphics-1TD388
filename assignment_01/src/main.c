//#define NO_STDIO_REDIRECT
#include <utilities/logging.h>
#include <utilities/file.h>
#include <time.h>
#include <glu.h>
#include <SDL2/SDL.h>

volatile int SHOULD_EXIT = 0;
volatile unsigned currentProgram = 0;

// The attribute locations we will use in the vertex shader
enum AttributeLocation {
    POSITION = 0,
    COLOR = 1
};

GLuint progamFromFilenames(const char* vertexShaderFile, const char *fragmentShaderFile){
  const char *shader_log = NULL;
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
  //glBindFragDataLocation(shaderProgram[0], 0, "frag_color");
  glLinkProgram(shaderProgram);
  return shaderProgram;
}

SDL_sem *sem_renderer_started;
int renderer(void *args){
  SDL_Window *win = (SDL_Window*)args;
  SDL_GLContext glcontext = SDL_GL_CreateContext(win);
  if (glcontext == NULL){
    report(FAIL, "SDL_GL_CreateContext Error: %s", SDL_GetError());
    SHOULD_EXIT = 1;
    SDL_SemPost(sem_renderer_started);
    return -1;
  }

  glewExperimental = GL_TRUE;
  GLenum glewStatus = glewInit();//correct?
  if (glewStatus != GLEW_OK) {
    report(FAIL, "Failed to initialize GLEW: %s (%d)", glewGetErrorString(glewStatus), glewStatus);
    return -1;
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  if(-1 == SDL_GL_SetSwapInterval(-1)){
    report(INFO,"Late tearing not supported!");
    if(-1== SDL_GL_SetSwapInterval(1)){
      report(WARN,"VSync not supported?!");
    }
  }

  GLuint shaderProgram[] = {
    progamFromFilenames("../shaders/position_2d.vert", "../shaders/red.frag"),
    progamFromFilenames("../shaders/position_color_2d.vert", "../shaders/color.frag"),
    progamFromFilenames("../shaders/position_color_time_2d.vert", "../shaders/color.frag")
  };

  const GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };

  const GLfloat vertices[] = {
      0.0f, 0.5f,
      -0.5f,-0.5f,
      0.5f,-0.5f
  };

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo); // Generate 1 buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);


  //GLint posAttrib = glGetAttribLocation(shaderProgram[0], "a_position");
  glVertexAttribPointer(POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(POSITION);
  glVertexAttribPointer(COLOR, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));
  glEnableVertexAttribArray(COLOR);

  GLint utime_loc = glGetUniformLocation(shaderProgram[2], "u_time");
  if(-1 == utime_loc){
    report(FAIL, "Couldn't find the location of u_time!");
  }

  SDL_SemPost(sem_renderer_started);
  uint32_t tick, minimum_ticks = 16.6; //10ms = 100 Hz
  glClearColor(0.4,0.6,0.8,1.0);
  do{tick = SDL_GetTicks();

    glUseProgram(shaderProgram[currentProgram]);
    if(2 == currentProgram){
      float now = (SDL_GetTicks() % 16000)*0.001f; //modulo to prevent rounding error.
      glUniform1f(utime_loc, now);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    SDL_GL_SwapWindow(win);
    uint32_t elapsed_ticks = SDL_GetTicks()-tick;
    if(elapsed_ticks < minimum_ticks){
      uint32_t sleep = minimum_ticks-elapsed_ticks;
      SDL_Delay(sleep);
    }
  }while(!SHOULD_EXIT);
  report(INFO, "Shutting down render_thread");
}

int main(int argc, char** argv){
  set_cwdir_to_bin_dir();
  //First we need to start up SDL, and make sure it went ok
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    report(FAIL, "SDL_Init Error: %s", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  if (SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)){
    report(FAIL, "SDL_Init Error: %s", SDL_GetError());
    return 1;
  }
  SDL_Window *win = SDL_CreateWindow("Assignment 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL\
  /*| SDL_WINDOW_BORDERLESS*/); //can also se to be fullscreen-desktop, borderless, input grabbed or high DPI if supported.
  if (win == NULL){
    report(FAIL, "SDL_CreateWindow Error: %s", SDL_GetError());
    return 1;
  }
  sem_renderer_started = SDL_CreateSemaphore(0);
  SDL_Thread* render_thread = SDL_CreateThread(&renderer, "renderer", win);
  SDL_SemWait(sem_renderer_started);
  uint32_t tick, minimum_ticks = 10; //10ms = 100 Hz
  SDL_Event event;
  uint32_t fullscreen_mode = 0;
  do{tick = SDL_GetTicks();
    while(SDL_PollEvent(&event)){
      switch(event.type){
        // look for a keypress
      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_1){
          if(0 != currentProgram)
            currentProgram = 0;
        }else if(event.key.keysym.sym == SDLK_2){
          if(1 != currentProgram)
            currentProgram = 1;
        }else if(event.key.keysym.sym == SDLK_3){
          if(2 != currentProgram)
            currentProgram = 2;
        }else if(event.key.keysym.sym == SDLK_F11){
          SDL_SetWindowFullscreen(win, fullscreen_mode ^= SDL_WINDOW_FULLSCREEN);
        }
      break;
      }
    }

    uint32_t elapsed_ticks = SDL_GetTicks()-tick;
    if(elapsed_ticks < minimum_ticks){
      uint32_t sleep = minimum_ticks-elapsed_ticks;
      SDL_Delay(sleep);
    }else{
      report(WARN, "Main loop can't keep up!");
    }
	}while (!(SHOULD_EXIT=SDL_QuitRequested())); //pumps events and stops if a quit was issued.
  report(INFO,"Shutting down all threads...");
  SDL_WaitThread(render_thread, NULL);
  report(INFO,"All threads joined");
  SDL_Quit();
}


/*
STRUCTURE:
  *initialize
    fork->draw_thread, runs at predetermined rate B. (vsync?)
    fork->worker threads? Handle any heavy calculating & async work.
    game_loop(), runs at predetermined rate A.
      game logic and input handling. pump_events at start for keyboard state, scan q for relevant presses.
*/
