#include "akane/light.h"
#include "akane/scene.h"

namespace akane
{
	bool VisibilityTester::Test(const Scene& scene) const
	{
		IntersectionInfo isect;
		if (scene.GetWorld().Intersect(TestRay(), 0.001f, 10000.f, isect))
		{
			if (isect.primitive == object_ && (isect.point - point_object_).LengthSq() < 0.001f)
			{
				return true;
			}
		}

		return false;
	}
}