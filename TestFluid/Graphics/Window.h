#pragma once

#include "../main.h"

class Window
{
public:
	Window(GLFWframebuffersizefun fun, int width, int height);
	Window(GLFWframebuffersizefun fun);
	bool isClosed();
	void swapBuffers();
	~Window();

	GLFWwindow* window;
private:

};