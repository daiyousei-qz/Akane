#pragma once

namespace akane
{
	class LocalCoordinateTransform
	{
	public:
		LocalCoordinateTransform(const Vec3f& xx, const Vec3f& yy, const Vec3f&& zz)
			: vx_(xx), vy_(yy), vz_(zz)
		{
		}
		LocalCoordinateTransform(const Vec3f& n)
		{
			Vec3f hh = (n.X() == 0.f) ? Vec3f{ 1.f, 0, 0 } : Vec3f{ 0, 1.f, 0 };
			Vec3f yy = n.Cross(hh);
			Vec3f xx = yy.Cross(n);

			vx_ = xx.Normalized();
			vy_ = yy.Normalized();
			vz_ = n.Normalized();

			AKANE_ASSERT((vx_.Cross(vy_) - vz_).LengthSq() < 0.01f);
		}

		const Vec3f & WorldX() const noexcept
		{
			return vx_;
		}
		const Vec3f& WorldY() const noexcept
		{
			return vy_;
		}
		const Vec3f& WorldZ() const noexcept
		{
			return vz_;
		}

		Vec3f WorldToLocal(const Vec3f & v) const noexcept
		{
			return Vec3f{ v.Dot(vx_), v.Dot(vy_), v.Dot(vz_) };
		}
		Vec3f LocalToWorld(const Vec3f & v) const noexcept
		{
			auto xx = v.X() * vx_.X() + v.Y() * vy_.X() + v.Z() * vz_.X();
			auto yy = v.X() * vx_.Y() + v.Y() * vy_.Y() + v.Z() * vz_.Y();
			auto zz = v.X() * vx_.Z() + v.Y() * vy_.Z() + v.Z() * vz_.Z();

			return Vec3f{ xx, yy, zz };
		}

	private:
		// BSDF local base vector
		// x, y, z, corespondingly
		// NOTE vz_ is also normal vector
		Vec3f vx_, vy_, vz_;
	};
}