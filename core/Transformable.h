//----	Sam Warren 2019	----
//3x3 column-major transformation matrix plus a 3D translation vector; xfm = 3x4 column major.

#pragma once
#include <maths/maths.h>

class Transformable {
	public:
		Affine3 xfm;

		Transformable() {}

		Transformable(const Affine3 &_xfm) : xfm(_xfm) {}
		
		inline void SetPosition(const Vec3 &_pos) {
			xfm[9] = _pos.x;
			xfm[10] = _pos.y;
			xfm[11] = _pos.z;
		}

		inline void SetScale(const Vec3 &_scale) {
			xfm[0] = _scale.x;
			xfm[4] = _scale.y;
			xfm[8] = _scale.z;
		}

		inline void SetEulerAngles(const Vec3 &_eulers) {
			xfm = Affine3::GetRotationX(_eulers.x) * Affine3::GetRotationY(_eulers.y) *Affine3::GetRotationZ(_eulers.z);
		}

		inline void AddChild(Transformable *_child) {
			children.push_back(_child);
			_child->parent = this;
		}

		//Get world-space affine transformation matrix.
		inline Affine3 GetAffine() const {
			return parent ? parent->xfm * this->xfm : xfm;
		}

	private:
		std::vector<Transformable*> children;
		Transformable *parent;
};