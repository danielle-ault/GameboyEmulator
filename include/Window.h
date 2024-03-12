#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int width, int height, const char* title, GLFWframebuffersizefun resizeCallback, GLFWkeyfun keyPressCallback);
	
	int ShouldClose();
	void SwapBuffersAndPollEvents();
	void Close();
	
private:
	GLFWwindow* GLFW_window;
};

