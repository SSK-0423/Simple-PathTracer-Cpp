#pragma once
#include "Vector.hpp"
#include "Quaternion.h"
#include <assert.h>
#include <stdio.h>

/// <summary>
/// 行列クラス(列優先)
/// </summary>
struct Matrix4x4 {
	Matrix4x4()
		: m_val{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 }
	{};

	Matrix4x4(float val[4][4])
		: m_val{
		val[0][0], val[0][1], val[0][2], val[0][3],
		val[1][0], val[1][1], val[1][2], val[1][3],
		val[2][0], val[2][1], val[2][2], val[2][3],
		val[3][0], val[3][1], val[3][2], val[3][3] }
	{}
	/// <summary>
	/// 列ベクトルで初期化
	/// </summary>
	Matrix4x4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3)
		: m_val{
		col0[0],col1[0],col2[0],col3[0],
		col0[1],col1[1],col2[1],col3[1],
		col0[2],col1[2],col2[2],col3[2],
		col0[3],col1[3],col2[3],col3[3] }
	{}
	~Matrix4x4() = default;

	// 行・列
	float m_val[4][4];

	Matrix4x4 operator *(const Matrix4x4 mat) const {
		Matrix4x4 ret;
		for (size_t row = 0; row < 4; row++) {
			for (size_t col = 0; col < 4; col++) {
				ret.m_val[row][col] = Dot(this->Row(row), mat.Column(col));
			}
		}
		return ret;
	}

	Vector4 Row(size_t index) const {
		assert(index < 4);

		return Vector4(m_val[index][0], m_val[index][1], m_val[index][2], m_val[index][3]);
	}
	Vector4 Column(size_t index) const {
		assert(index < 4);

		return Vector4(m_val[0][index], m_val[1][index], m_val[2][index], m_val[3][index]);
	}

	Vector4 operator *(const Vector4 vec) {
		Vector4 ret;
		ret.x = Dot(this->Row(0), vec);
		ret.y = Dot(this->Row(1), vec);
		ret.z = Dot(this->Row(2), vec);
		ret.w = Dot(this->Row(3), vec);
		return ret;
	}

	Matrix4x4 Transpose() const {
		Vector4 col0 = Vector4(m_val[0][0], m_val[0][1], m_val[0][2], m_val[0][3]);
		Vector4 col1 = Vector4(m_val[1][0], m_val[1][1], m_val[1][2], m_val[1][3]);
		Vector4 col2 = Vector4(m_val[2][0], m_val[2][1], m_val[2][2], m_val[2][3]);
		Vector4 col3 = Vector4(m_val[3][0], m_val[3][1], m_val[3][2], m_val[3][3]);

		return Matrix4x4(col0, col1, col2, col3);
	}

	// 行列の表示
	void Show() {
		for (size_t row = 0; row < 4; row++) {
			printf("|%6.2f %6.2f %6.2f %6.2f|\n", m_val[row][0], m_val[row][1], m_val[row][2], m_val[row][3]);
		}
		printf("\n");
	}

	static Matrix4x4 Identity() {
		return Matrix4x4(
			Vector4(1, 0, 0, 0),
			Vector4(0, 1, 0, 0),
			Vector4(0, 0, 1, 0),
			Vector4(0, 0, 0, 1));
	}

	static Matrix4x4 Translate(const float& x, const float& y, const float& z) {
		return Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(x, y, z, 1.f));
	}

	/// <summary>
	/// 左手座標系のZ⇒X⇒Yの順で回転
	/// </summary>
	/// <param name="deg"></param>
	/// <returns></returns>
	static Matrix4x4 RotationZXY(const float& degX, const float& degY, const float& degZ) {
		// 各軸のクォータニオンを求めて合成する
		Quaternion qx = Quaternion(Vector3(1, 0, 0) * sinf(DegToRad(degX / 2.f)), cosf(DegToRad(degX / 2.f)));
		Quaternion qy = Quaternion(Vector3(0, 1, 0) * sinf(DegToRad(degY / 2.f)), cosf(DegToRad(degY / 2.f)));
		Quaternion qz = Quaternion(Vector3(0, 0, 1) * sinf(DegToRad(degZ / 2.f)), cosf(DegToRad(degZ / 2.f)));
		Quaternion rot = qz * qx * qy;

		// 求めたクォータニオンを回転行列に変換する
		float mat[4][4];
		mat[0][0] = 2.f * powf(rot.w, 2.f) + 2.f * powf(rot.x(), 2.f) - 1.f;
		mat[0][1] = 2.f * rot.x() * rot.y() - 2.f * rot.z() * rot.w;
		mat[0][2] = 2.f * rot.x() * rot.z() + 2.f * rot.y() * rot.w;
		mat[0][3] = 0.f;

		mat[1][0] = 2.f * rot.x() * rot.y() + 2.f * rot.z() * rot.w;
		mat[1][1] = 2.f * powf(rot.w, 2.f) + 2.f * powf(rot.y(), 2.f) - 1.f;
		mat[1][2] = 2.f * rot.y() * rot.z() - 2.f * rot.x() * rot.w;
		mat[1][3] = 0.f;

		mat[2][0] = 2.f * rot.x() * rot.z() - 2.f * rot.y() * rot.w;
		mat[2][1] = 2.f * rot.y() * rot.z() + 2.f * rot.x() * rot.w;
		mat[2][2] = 2.f * powf(rot.w, 2.f) + 2.f * powf(rot.z(), 2.f) - 1.f;
		mat[2][3] = 0.f;

		mat[3][0] = 0.f;
		mat[3][1] = 0.f;
		mat[3][2] = 0.f;
		mat[3][3] = 1.f;

		return Matrix4x4(mat);
	}

	static Matrix4x4 Scalling(const float& scaleX, const float& scaleY, const float& scaleZ) {
		// 負数は0にする
		return Matrix4x4(
			Vector4(Max(scaleX, 0.f), 0, 0, 0),
			Vector4(0, Max(scaleY, 0.f), 0, 0),
			Vector4(0, 0, Max(scaleZ, 0.f), 0),
			Vector4(0, 0, 0, 1));
	}
};