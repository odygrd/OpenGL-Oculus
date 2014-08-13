// GLFWOculusRiftTest
// Based on OculusSDK 3.0.2 Preview
// Windows

#include "display.hpp"
#include "scene.hpp"

int main(void)
{

	Display* display = new Display(1280,800,"Oculus Rift Demo");
	display->Init();

	Scene* scene = new Scene();
	scene->Init();

	while (!display->IsClosed())
	{
		display->Update(scene);
	}

	delete scene;
	delete display;


	exit(EXIT_SUCCESS);
}
