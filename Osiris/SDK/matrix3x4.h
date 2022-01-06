#pragma once

struct Vector;

class matrix3x4 {
public:
    float mat[3][4];
    constexpr auto operator[](int i) const noexcept { return mat[i]; }
    auto operator[](int i) noexcept { return mat[i]; }
    constexpr auto origin() const noexcept;
};

#include "Vector.h"

constexpr auto matrix3x4::origin() const noexcept
{
    return Vector{ mat[0][3], mat[1][3], mat[2][3] };
}

class BoneArray : public matrix3x4 {
public:
	bool get_bone(Vector& out, int bone = 0) {
		if (bone < 0 || bone >= 128)
			return false;

		matrix3x4* bone_matrix = &this[bone];

		if (!bone_matrix)
			return false;

		out = { bone_matrix->mat[0][3], bone_matrix->mat[1][3], bone_matrix->mat[2][3] };

		return true;
	}
};
