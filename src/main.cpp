#include "chunk.h"
#include "chunkmanager.h"
#include "frustum.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// TODO: Remove globals
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
ChunkManager *manager = NULL;
FrustumG *frustum = NULL;

SDL_AppResult key_callback(SDL_KeyboardEvent *event) {
  const float cameraSpeed = 10;

  if (event->key == SDLK_W && event->type == SDL_EVENT_KEY_DOWN)
    cameraPos -= cameraSpeed * cameraFront;
  if (event->key == SDLK_S && event->type == SDL_EVENT_KEY_DOWN)
    cameraPos += cameraSpeed * cameraFront;
  if (event->key == SDLK_A && event->type == SDL_EVENT_KEY_DOWN)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (event->key == SDLK_D && event->type == SDL_EVENT_KEY_DOWN)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (event->key == SDLK_ESCAPE && event->type == SDL_EVENT_KEY_DOWN) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

float pitch = 0.0f, yaw = 0.0f;

static Shader *shader;

void mouse_callback(SDL_MouseMotionEvent *event) {
  float xoffset = event->xrel;
  float yoffset = event->yrel;
  float sensitivity = 0.05;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Voxel Engine", 640, 480, SDL_WINDOW_OPENGL,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (glewInit() != GLEW_OK) {
    SDL_Log("Couldn't initialize GLEW");
    return SDL_APP_FAILURE;
  }

  shader = new Shader("assets/shaders/shader.vs", "assets/shaders/shader.fs");
  frustum = new FrustumG();
  manager = new ChunkManager(frustum);

  SDL_GL_SetSwapInterval(1);
  glClearColor(0.0f, 0.2f, 0.8f, 1.0f);
  SDL_CaptureMouse(true);

  SDL_SetWindowRelativeMouseMode(window, true);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    return key_callback((SDL_KeyboardEvent *)event);
  case SDL_EVENT_MOUSE_MOTION:
    mouse_callback((SDL_MouseMotionEvent *)event);
    break;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  glm::vec3 cameraDir = cameraPos + cameraFront;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader->use();
  const glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(cameraDir, cameraPos, cameraUp);
  frustum->setCamDef(cameraPos, cameraDir, cameraUp);

  manager->Update(0, cameraPos, cameraDir);

  int viewLoc = glGetUniformLocation(shader->ID, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  int projectionLoc = glGetUniformLocation(shader->ID, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  manager->Render(*shader);

  SDL_GL_SwapWindow(window);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (result == SDL_APP_FAILURE) {
    SDL_Log("Error: %s", SDL_GetError());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  delete shader;
  delete manager;
  delete frustum;
}
