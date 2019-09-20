#include "akane/light.h"
#include "akane/primitive.h"
#include "akane/scene.h"

namespace akane
{
    bool LightSample::TestVisibility(const Scene& scene, Workspace& workspace, const Vec3& p,
                                     const Primitive* obj) const
    {
        auto test_ray = GenerateTestRay(p);
        if (normal_ != Vec3{0.f} && Dot(normal_, test_ray.d) < 0)
        {
            // from back side of the light source
            return false;
        }

        IntersectionInfo isect;
        if (!scene.Intersect(GenerateTestRay(p), workspace, isect))
        {
            // no intersection with original primitive
            return false;
        }

        return SamePrimitive(obj, isect.object) && (isect.point - p).LengthSq() < 0.001f;
    }
} // namespace akane