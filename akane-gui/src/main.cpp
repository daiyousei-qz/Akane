#include "app.h"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    akane::gui::AkaneApp app;
    return RunApplication(app);
}
