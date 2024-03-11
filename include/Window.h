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

Window::Window(int width, int height, const char* title, GLFWframebuffersizefun resizeCallback, GLFWkeyfun keyPressCallback)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFW_window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (GLFW_window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		//return -1;
	}

	glfwMakeContextCurrent(GLFW_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		//return -1;
	}

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(GLFW_window, resizeCallback);
	
	glfwSetKeyCallback(GLFW_window, keyPressCallback);
}

inline int Window::ShouldClose()
{
	return glfwWindowShouldClose(GLFW_window);
}

inline void Window::SwapBuffersAndPollEvents()
{
	glfwSwapBuffers(GLFW_window);
	glfwPollEvents();
}

inline void Window::Close()
{
	glfwTerminate();
}
