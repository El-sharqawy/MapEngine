#include "WindowManager.h"
#include "CoordTransform.h"
#include <memory>

int main(int argc, char* argv[])
{
	std::unique_ptr<CWindowManager> pWindowApp = std::make_unique<CWindowManager>();
	if (pWindowApp->InitializeWindow("Hello World") == false)
	{
		syserr("Failed to Initialize Window");
		return (EXIT_FAILURE);
	}

	pWindowApp->Update();

	glfwTerminate();

	return (EXIT_SUCCESS);
}