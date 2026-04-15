#include "Window.h"
#include <memory>

int main(int argc, char* argv[])
{
	std::unique_ptr<CWindow> pWindow = std::make_unique<CWindow>();
	if (pWindow->InitializeWindow("Hello World") == false)
	{
		syserr("Failed to Initialize Window");
		return (EXIT_FAILURE);
	}

	while (glfwWindowShouldClose(pWindow->GetGLWindow()) == false)
	{
		glfwPollEvents();

		glfwSwapBuffers(pWindow->GetGLWindow());
	}

	glfwTerminate();

	return (EXIT_SUCCESS);
}