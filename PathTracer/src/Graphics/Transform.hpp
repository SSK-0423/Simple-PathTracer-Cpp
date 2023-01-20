#pragma once
#include "../Utility/PathTracerMath.hpp"

namespace PathTracer {
	class Transform {
	public:
		Transform()
			: m_position(0, 0, 0), m_degRotation(0, 0, 0), m_scale(1, 1, 1)
		{}
		Transform(const Vector3& pos, const Vector3& degRot = Vector3(0, 0, 0), const Vector3& scale = Vector3(1, 1, 1))
			: m_position(pos), m_degRotation(degRot), m_scale(scale)
		{}
		~Transform() = default;

		const Matrix4x4 GetModelMatrix() const {
			Matrix4x4 translation = Matrix4x4::Translate(m_position.x, m_position.y, m_position.z);
			Matrix4x4 rotation = Matrix4x4::RotationZXY(m_degRotation.x, m_degRotation.y, m_degRotation.z);
			Matrix4x4 scalling = Matrix4x4::Scalling(m_scale.x, m_scale.y, m_scale.z);

			// ïœä∑èáèò
			// ÉTÉCÉYÅÀâÒì]ÅÀà⁄ìÆ
			return translation * rotation * scalling;
		}

		void SetPosition(const Vector3& pos) { m_position = pos; }
		void SetDegRotation(const Vector3& degRot) { m_degRotation = degRot; }
		void SetScale(const Vector3& scale) { m_scale = scale; }
	private:
		Vector3 m_position;
		Vector3 m_degRotation;
		Vector3 m_scale;
	};
}