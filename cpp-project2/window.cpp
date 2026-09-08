#include <glad/gl.h>
#include "window.hpp"
#include "GLFW/glfw3.h"
#include <stdexcept>
#include <string>

Window::Window(const int width, const int height, bool vsync) : width(width), height(height) {
    logger->info("Trying OpenGL 4.6");
	initializeWindow();
    logger->info("Window initialized");

    if (vsync) {
        glfwSwapInterval(1); 
    }
    
}

Window::~Window() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

void Window::initializeWindow() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "C++ Game", nullptr, nullptr);
	if (window == NULL) {


		const char* description;
		int code = glfwGetError(&description);

		throw std::runtime_error("Window creation failure: (" + std::to_string(code) + "): " + std::string(description));
	}
	glfwMakeContextCurrent(window);


	// Load GLAD
	int version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
	if (!version) {
		throw std::runtime_error("GLAD initialization failed");
	}
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}

bool Window::checkKeyPressed(unsigned int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

Coord Window::getMousePosition() const {

    double xpos;
    double ypos;

    glfwGetCursorPos(window, &xpos, &ypos);

    return Coord{
        .x = (xpos),
        .y = (ypos)
    };
}

void Window::lockMouse() {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported()) {
        logger->info("Using raw mouse motion");
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    } else {
        logger->info("Raw mouse motion is unsupported");
    }
}