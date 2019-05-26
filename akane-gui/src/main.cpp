#include "akane_app.h"

#include "windows.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	akane::gui::AkaneApp app;
	return RunApplication(app);
}
