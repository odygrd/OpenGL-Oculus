#ifndef DISPLAY_H
#define DISPLAY_H
#include <GL\glew.h>

#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32 //Window API
#define GLFW_EXPOSE_NATIVE_WGL //Context API

#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h> // Native Access

#include <LibOVR\Include\OVR.h>
#include <LibOVR\Src\OVR_CAPI.h>
#include <LibOVR\Src\OVR_CAPI_GL.h>

#include <iostream>
#include <string>
#include <algorithm>

#include "scene.hpp"

class Display
{
public:
	Display(int width, int height, const std::string& title);
	virtual ~Display();

	void Init();
	void Update(Scene* scene);
	bool IsClosed();

	void InitOculus();
	void InitGLFW();
	void RenderToTexture();
	void ConfgOculus();

	void PrintWindowInfo(GLFWwindow* p_Window);
	
	static void Error_callback(int error, const char* description);
	static void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void WindowSizeCallback(GLFWwindow* p_Window, int p_Width, int p_Height);
private:
	//Definitions
	const glm::vec3 X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 CAMERA = glm::vec3(0.0f, -7.0f, 1.0f);
	const glm::vec3 ORIGIN = glm::vec3(0.0f, -7.0f, 0.0f);
	const glm::vec3 UP = Y_AXIS;

	const bool FullScreen = false;
	const bool MultiSampling = false;
	const int samples = 4;

	//GLFW window Variables
	int _width, _height;
	std::string _title;
	GLFWwindow* _window;
	bool _isClosed;

	//Oculus Variables
	ovrHmdDesc _hmdDesc;
	ovrSizei _clientSize;
	OVR::Sizei _renderTargetSize;
	ovrGLTexture _eyeTexture[2];

	GLuint _renderTextureId;
	GLuint _depthBufferId;
	GLuint _frammeBufferId;
};
#endif