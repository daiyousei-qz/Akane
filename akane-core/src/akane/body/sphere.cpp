#include "akane/core.h"
#include "akane/body.h"

namespace akane
{
    class Sphere : public Body
    {
    public:
        Sphere(Vec3f center, akFloat radius)
            : center_(center), radius_(radius){};

        bool Intersect(const Ray& ray, akFloat t_min, akFloat t_max,
                       IntersectionInfo& info) const override
        {
            // ray origin
            Vec3f ray_o = ray.o;
            // ray direction
            Vec3f ray_d = ray.d.Normalized();
            // ray origin to sphere center
            Vec3f l = center_ - ray_o;

            // length of l's project onto ray's direction
            // NOTE length of the projection decrease as ray points away from
            // center of the sphere
            akFloat s    = l.Dot(ray_d);
            akFloat l_sq = l.Dot(l);
            akFloat r_sq = radius_ * radius_;
            if (s * s < l_sq - r_sq || l_sq < r_sq)
            {
                return false; // ray outside does not points to sphere
            }

            akFloat m_sq = l_sq - s * s;
            akFloat q    = sqrt(r_sq - m_sq);
            akFloat t    = l_sq > r_sq ? s - q : s + q;

            if (t < t_min || t > t_max) { return false; }

            info.t      = t;
            info.point  = ray_o + t * ray_d;
            info.normal = (info.point - center_) / radius_;
            return true;
        }

    private:
        Vec3f center_;
        akFloat radius_;
    };
} // namespace akane