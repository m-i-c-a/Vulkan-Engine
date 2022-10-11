#include <GLFW/glfw3.h>

class InputSystem;

void setInputSystem(InputSystem* inputSys);

void glfw_cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

void glfw_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);