#include "akane/math/vector.h"
#include "external/imgui/imgui.h"
#include <functional>

#include <d3d11.h>

void InitializeDisplayTexture(int width, int height, bool override = false);
void UpdateDisplayTexture(const std::function<akane::Point3i(int x, int y)>& pixel_source);
ImTextureID RetriveDisplayTexture();