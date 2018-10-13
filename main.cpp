// Minimal OpenGL 2 + SDL 2 example.
// Run with:
// $ g++ main.cpp `pkg-config gl sdl2 --cflags --libs` && ./a.out
#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include <cassert>

static const char* vertex_shader =
  "#version 130\n"
  "in vec2 pos;\n"
  "out vec4 v_color;\n"
  "void main() {\n"
  "    v_color = vec4(pos, 1.0 - pos.x, 0);\n"
  "    gl_Position = vec4( pos, 0.0, 1.0 );\n"
  "}\n";

static const char* fragment_shader =
  "#version 130\n"
  "in vec4 v_color;\n"
  "out vec4 o_color;\n"
  "void main() {\n"
  "    o_color = v_color;\n"
  "}\n";

enum { attrib_position };

int createShader(int type, const char* code)
{
  auto vs = glCreateShader(type);

  glShaderSource(vs, 1, &code, nullptr);
  glCompileShader(vs);

  GLint status;
  glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
  assert(status);
  return vs;
}

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE | SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  auto window = SDL_CreateWindow("Minimal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  auto context = SDL_GL_CreateContext(window);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  auto vs = createShader(GL_VERTEX_SHADER, vertex_shader);
  auto fs = createShader(GL_FRAGMENT_SHADER, fragment_shader);

  auto program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glBindAttribLocation(program, attrib_position, "pos");
  glLinkProgram(program);

  glUseProgram(program);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(attrib_position);
  glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(0 * sizeof(float)));

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

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

  for(int i = 0; i < 3; ++i)
  {
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(*vertices));

    SDL_GL_SwapWindow(window);
    SDL_Delay(100);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

