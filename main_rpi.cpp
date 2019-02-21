// Minimal OpenGL ES 2 + SDL 2 example on Raspberry pi.
//
// Run with:
// $ g++ main_rpi.cpp `pkg-config brcmglesv2 sdl2 --cflags --libs` && ./a.out
//
// Remember to add the following to PKG_CONFIG_PATH:
// * /opt/vc/lib/pkgconfig (for brcmglesv2 package)

#include <SDL.h>
#include <SDL_opengles2.h>
#include <cassert>

// beware, the broadcom driver will only do the actual compilation when calling glUseProgram.
static const char* vertex_shader =
    "attribute vec4 vertex;\n"
    "void main(void) {\n"
    " vec4 pos = vertex\n;"
    " gl_Position = pos\n;"
    "}";

static const char* fragment_shader =
  "void main() {\n"
  "    gl_FragColor = vec4(1, 0, 0, 1);\n"
  "}\n";

enum { attrib_position };

void safeGl(const char* call, const char* path, int line)
{
  auto err = glGetError();
  if(err)
  {
    fprintf(stderr, "[%s:%d] %s returned %d\n", path, line, call, err);
    exit(1);
  }
}

#define SAFEGL(call) \
  do { call; safeGl(#call, __FILE__, __LINE__); } while(0)

int createShader(int type, const char* code)
{
  auto vs = glCreateShader(type);

  SAFEGL(glShaderSource(vs, 1, &code, nullptr));
  SAFEGL(glCompileShader(vs));

  GLint status;
  SAFEGL(glGetShaderiv(vs, GL_COMPILE_STATUS, &status));
  assert(status);
  return vs;
}

int main()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE | SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  auto window = SDL_CreateWindow("Minimal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  assert(window);
  auto context = SDL_GL_CreateContext(window);
  assert(context);

  auto vs = createShader(GL_VERTEX_SHADER, vertex_shader);
  auto fs = createShader(GL_FRAGMENT_SHADER, fragment_shader);

  auto program = glCreateProgram();
  SAFEGL(glAttachShader(program, vs));
  SAFEGL(glAttachShader(program, fs));

  SAFEGL(glBindAttribLocation(program, attrib_position, "pos"));
  SAFEGL(glLinkProgram(program));

  SAFEGL(glUseProgram(program));

  GLuint vbo;
  SAFEGL(glGenBuffers(1, &vbo));
  SAFEGL(glBindBuffer(GL_ARRAY_BUFFER, vbo));

  SAFEGL(glEnableVertexAttribArray(attrib_position));
  SAFEGL(glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0 * sizeof(float))));

  struct Vertex
  {
    float x, y;
  };

  const Vertex vertices[] =
  {
    { -1, -1 },
    { -1, +1 },
    { +1, -1 },

    { -1, -1 },
    { +1, +1 },
    { +1, -1 },
  };

  SAFEGL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW));

  for(int i = 0; i < 5; ++i)
  {
    SAFEGL(glClearColor(0, 1, 0, 1));
    SAFEGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    SAFEGL(glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(*vertices)));

    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

