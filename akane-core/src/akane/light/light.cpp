#include "akane/light.h"
#include "akane/scene.h"

namespace akane
{
    bool VisibilityTester::Test(const Scene& scene, Workspace& workspace) const
    {
        IntersectionInfo isect;
        if (scene.Intersect(TestRay(), workspace, isect))
        {
            if (SamePrimitive(object_, isect.primitive) &&
                (isect.point - object_point_).LengthSq() < 0.001f)
            {
                return true;
            }
        }

        return false;
    }
} // namespace akane