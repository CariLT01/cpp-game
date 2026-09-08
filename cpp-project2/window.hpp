#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

struct Coord {
    double x;
    double y;
};

class Window {
public:
	Window(const int width, const int height, bool vsync = true);
	~Window();

	void swapBuffers();
	void pollEvents();

    void lockMouse();

	bool shouldClose() const;

    bool checkKeyPressed(unsigned int key) const;

    Coord getMousePosition() const;


private:
	void initializeWindow();

	int width;
	int height;

	GLFWwindow* window;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("window");
};