#include "display.hpp"

//Globals needed for resize callback
ovrHmd hmd;
ovrGLConfig cfg;
ovrFovPort eyeFov[2];
ovrEyeRenderDesc eyeRenderDesc[2];
extern ShaderProgram shpMain;

using namespace std;

Display::Display(int width, int height, const std::string& title) : _width(width), _height(height), _title(title), _isClosed(false)
{}

Display::~Display()
{
	glDeleteFramebuffers(1, &_frammeBufferId);
	glDeleteTextures(1, &_renderTextureId);
	glDeleteRenderbuffers(1, &_depthBufferId);

	glfwDestroyWindow(_window);
	glfwTerminate();

	ovrHmd_Destroy(hmd);
	ovr_Shutdown();
}

void Display::InitOculus()
{
	// Initialize LibOVR...
	ovr_Initialize();
	
	hmd = ovrHmd_Create(0);
	//If no Rift is plugged in, create a virtual HMD of the speciﬁed type - no sensor input
	if (!hmd)
	{
		hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
	}

	// fill hmdDesc structure
	ovrHmd_GetDesc(hmd, &_hmdDesc);

	//Initialize the sensors with orientation, yaw correction, and position tracking capabilities enabled if available, while actually requiring that only basic orientation tracking be present
	ovrHmd_StartSensor(hmd, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection | ovrSensorCap_Position, ovrSensorCap_Orientation);

	//Configure Stereo Settings
	OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, _hmdDesc.DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, _hmdDesc.DefaultEyeFov[1], 1.0f);

	//Create a shared texture, large enough to fit both eye renderings
	_renderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
	_renderTargetSize.h = std::max(recommenedTex0Size.h, recommenedTex1Size.h);
}

void Display::InitGLFW()
{
	if (!glfwInit())
	{
		cerr << "Failed to initialize GLFW" << endl;
		exit(EXIT_FAILURE);
	}

	(MultiSampling) ? glfwWindowHint(GLFW_SAMPLES, samples) : glfwWindowHint(GLFW_SAMPLES, 0);
	if (FullScreen)
	{
		// Create a fullscreen window with the Oculus Rift resolution...
		_clientSize = _hmdDesc.Resolution; // 1280 for DK1...
		_window = glfwCreateWindow(_clientSize.w, _clientSize.h, "GLFW Oculus Rift Test", glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		_clientSize.w = _width;
		_clientSize.h = _height;
		_window = glfwCreateWindow(_clientSize.w, _clientSize.h, "GLFW Oculus Rift Test", NULL, NULL);
	}

	glfwSetErrorCallback(Error_callback);

	if (!_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the context current for this window...
	glfwMakeContextCurrent(_window);
	//Set key and Window Callbacks
	glfwSetKeyCallback(_window, Key_callback);
	glfwSetWindowSizeCallback(_window, WindowSizeCallback);

	// Initialize GLEW
	glewExperimental = GL_TRUE; //Turn glewExperimental on to avoid problem fetching function pointers...
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
}

void Display::ConfgOculus()
{
	// Oculus Rift eye configurations...
	cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	cfg.OGL.Header.Multisample = (MultiSampling ? 1 : 0);
	cfg.OGL.Header.RTSize = OVR::Sizei(_clientSize.w, _clientSize.h);
	cfg.OGL.Window = glfwGetWin32Window(_window);

	// Initialize eye rendering information for ovrHmd_Configure.
	// The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
	eyeFov[0] = _hmdDesc.DefaultEyeFov[0];
	eyeFov[1] = _hmdDesc.DefaultEyeFov[1];

	ovrRecti eyeRenderViewport[2];
	eyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
	eyeRenderViewport[0].Size = OVR::Sizei(_renderTargetSize.w / 2, _renderTargetSize.h);
	eyeRenderViewport[1].Pos = OVR::Vector2i((_renderTargetSize.w + 1) / 2, 0);
	eyeRenderViewport[1].Size = eyeRenderViewport[0].Size;

	_eyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
	_eyeTexture[0].OGL.Header.TextureSize = _renderTargetSize;
	_eyeTexture[0].OGL.Header.RenderViewport = eyeRenderViewport[0];
	_eyeTexture[0].OGL.TexId = _renderTextureId;

	// Right eye the same, except for the x-position in the texture...
	_eyeTexture[1] = _eyeTexture[0];
	_eyeTexture[1].OGL.Header.RenderViewport = eyeRenderViewport[1];

	if (!ovrHmd_ConfigureRendering(hmd, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp, eyeFov, eyeRenderDesc))
		exit(EXIT_FAILURE);

}


void Display::Init()
{
	InitOculus();

	InitGLFW();

	PrintWindowInfo(_window);

	RenderToTexture();

	ConfgOculus();
}

void Display::Update(Scene* scene)
{
	if (glfwWindowShouldClose(_window))
		_isClosed = true;

	//Generate FBO Mipmaps
	glBindTexture(GL_TEXTURE_2D, _renderTextureId);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _frammeBufferId); // Switch to texture rendering

	// Clear...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ovrFrameTiming m_HmdFrameTiming = ovrHmd_BeginFrame(hmd, 0);

	for (int l_EyeIndex = 0; l_EyeIndex < ovrEye_Count; l_EyeIndex++)
	{
		ovrEyeType l_Eye = _hmdDesc.EyeRenderOrder[l_EyeIndex];
		ovrPosef l_EyePose = ovrHmd_BeginEyeRender(hmd, l_Eye);

		glViewport(_eyeTexture[l_Eye].OGL.Header.RenderViewport.Pos.x,
			_eyeTexture[l_Eye].OGL.Header.RenderViewport.Pos.y,
			_eyeTexture[l_Eye].OGL.Header.RenderViewport.Size.w,
			_eyeTexture[l_Eye].OGL.Header.RenderViewport.Size.h
			);

		//Print Orientation Angles
		OVR::Transformf pose = ovrHmd_GetSensorState(hmd, m_HmdFrameTiming.ScanoutMidpointSeconds).Predicted.Pose;
		float hmdYaw, hmdPitch, hmdRoll;
		pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&hmdYaw, &hmdPitch, &hmdRoll);
		//printf("yaw : %4.0f  pitch : %4.0f  roll : %4.0f\n", OVR::RadToDegree(hmdYaw), OVR::RadToDegree(hmdPitch), OVR::RadToDegree(hmdRoll));


		// Get Projection and ModelView matrices from the device...
		OVR::Matrix4f projectionMatrix = ovrMatrix4f_Projection(eyeRenderDesc[l_Eye].Fov, 0.01f, 10000.0f, true); //get projection matrix
		OVR::Matrix4f translationMatrix = OVR::Matrix4f::Translation(eyeRenderDesc[l_Eye].ViewAdjust); //get translation matrix for each eye
		OVR::Quatf orientationQuartention = OVR::Quatf(l_EyePose.Orientation); //get orientation matrix

		//Convert to GLM Matrices;
		glm::vec3 eulerAngles;
		orientationQuartention.GetEulerAngles<OVR::Axis_X, OVR::Axis_Y, OVR::Axis_Z, OVR::Rotate_CW, OVR::Handed_R>(&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);
		glm::quat orientationGLM = glm::quat(eulerAngles);
		glm::mat4 eyeTranslationGLM = glm::transpose(glm::make_mat4(&translationMatrix.M[0][0]));


		glm::mat4 modelviewGLM = glm::mat4_cast(orientationGLM) * glm::lookAt(CAMERA, ORIGIN, UP) * eyeTranslationGLM;
		glm::mat4 projectionGLM = glm::transpose(glm::make_mat4(&projectionMatrix.M[0][0]));

		scene->Render(projectionGLM, modelviewGLM);

		ovrHmd_EndEyeRender(hmd, l_Eye, l_EyePose, &_eyeTexture[l_Eye].Texture);

	}

	ovrHmd_EndFrame(hmd);
	glfwPollEvents();
}

bool Display::IsClosed()
{
	return _isClosed;
}

void Display::RenderToTexture()
{
	// Create a framebuffer object

	glGenFramebuffers(1, &_frammeBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _frammeBufferId);

	// Create a renderbuffer object to store depth info
	
	glGenRenderbuffers(1, &_depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _renderTargetSize.w, _renderTargetSize.h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Create a texture object
	glGenTextures(1, &_renderTextureId);
	glBindTexture(GL_TEXTURE_2D, _renderTextureId); // "Bind" the newly created texture : all future texture functions will modify this texture...
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _renderTargetSize.w, _renderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // Allocate space to the texture

	//Texture parameters Linear filtering...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach the texture to FBO color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTextureId, 0);
	// attach the renderbuffer to depth attachment point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufferId);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Check if everything is OK...
	GLenum l_Check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (l_Check != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("There is a problem with the FBO.\n");
		exit(EXIT_FAILURE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Display::Error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void Display::Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Display::WindowSizeCallback(GLFWwindow* p_Window, int p_Width, int p_Height)
{
	
	cfg.OGL.Header.RTSize.w = p_Width;
	cfg.OGL.Header.RTSize.h = p_Height;

	int l_DistortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;
	ovrHmd_ConfigureRendering(hmd, &cfg.Config, l_DistortionCaps, eyeFov, eyeRenderDesc);
	glUseProgram(shpMain.GetProgramID());
}

void Display::PrintWindowInfo(GLFWwindow* p_Window)
{
	int l_Major = glfwGetWindowAttrib(p_Window, GLFW_CONTEXT_VERSION_MAJOR);
	int l_Minor = glfwGetWindowAttrib(p_Window, GLFW_CONTEXT_VERSION_MINOR);
	int l_Profile = glfwGetWindowAttrib(p_Window, GLFW_OPENGL_PROFILE);
	printf("OpenGL Version : %d.%d ", l_Major, l_Minor);
	if (l_Major >= 3) // Profiles introduced in OpenGL 3.0...
	{
		if (l_Profile == GLFW_OPENGL_COMPAT_PROFILE) printf("GLFW_OPENGL_COMPAT_PROFILE\n"); else printf("GLFW_OPENGL_CORE_PROFILE\n");
	}
	printf("Vendor: %s\n", (char*)glGetString(GL_VENDOR));
	printf("Renderer: %s\n", (char*)glGetString(GL_RENDERER));
}


