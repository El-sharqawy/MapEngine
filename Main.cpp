#include "Window.h"
#include <memory>

int main(int argc, char* argv[])
{
	std::unique_ptr<CWindow> pWindowApp = std::make_unique<CWindow>();
	if (pWindowApp->InitializeWindow("Hello World") == false)
	{
		syserr("Failed to Initialize Window");
		return (EXIT_FAILURE);
	}

	pWindowApp->Update();

	glfwTerminate();

	return (EXIT_SUCCESS);
}