#include "Game.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

int factor = 155;
const extern unsigned int SCREEN_WIDTH = 3 * factor;
const extern unsigned int SCREEN_HEIGHT = 4 * factor;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GL OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  framebuffer_size_callback(window, width, height);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Breakout.Init();

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    Breakout.ProcessInput(deltaTime);
    Breakout.Update(deltaTime);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Breakout.Render();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (action == GLFW_PRESS)
      Breakout.OnMouseDown(xpos, ypos);
    if (action == GLFW_RELEASE)
      Breakout.OnMouseUp(xpos, ypos);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {

  float targetAspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

  float windowAspectRatio = (float)width / (float)height;

  int viewportWidth, viewportHeight;
  int xOffset, yOffset;

  if (windowAspectRatio > targetAspectRatio) {

    viewportHeight = height;
    viewportWidth = (int)(height * targetAspectRatio);
    xOffset = (width - viewportWidth) / 2;
    yOffset = 0;
  } else {

    viewportWidth = width;
    viewportHeight = (int)(width / targetAspectRatio);
    xOffset = 0;
    yOffset = (height - viewportHeight) / 2;
  }

  glViewport(xOffset, yOffset, viewportWidth, viewportHeight);
}
