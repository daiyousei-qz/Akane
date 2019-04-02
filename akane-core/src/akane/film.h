#pragma once
#include <vector>

namespace akane
{
	// film where a scene is rendered
	class Film
	{
    private:
        int width_;
        int height_;
        std::vector<float> pixels_;
	};
}