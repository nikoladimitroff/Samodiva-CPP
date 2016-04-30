#pragma once
#include <cstring>

#pragma warning(disable: 4201)

namespace Samodiva
{
	struct VectorPAD
	{
		union
		{
			struct
			{
				float P;
				float A;
				float D;
			};
			float _V[3];
		};

		VectorPAD(float p, float a, float d)
			: P(p)
			, A(a)
			, D(d)
		{}

		VectorPAD()
			: VectorPAD(0, 0, 0)
		{}

		inline float LengthSquared() const
		{
			return P * P + A * A + D * D;
		}
		inline void Normalize();
		inline void Abs()
		{
			P = fabs(P);
			A = fabs(A);
			D = fabs(D);
		}
	};
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper)
	{
		if (n < lower)
		{
			return lower;
		}
		else if (n > upper)
		{
			return upper;
		}
		return n;
	}

	inline bool EpsilonEquals(float a, float b, float epsilon = 1e-5f) {
		return (fabs(a - b) <= epsilon);
	}

	inline bool operator==(const VectorPAD& lhs, const VectorPAD& rhs)
	{
		return EpsilonEquals(lhs.P, rhs.P) && EpsilonEquals(lhs.A, rhs.A) && EpsilonEquals(lhs.D, rhs.D);
	}

	inline bool operator!=(const VectorPAD& lhs, const VectorPAD& rhs)
	{
		return !(lhs == rhs);
	}

	inline VectorPAD& operator+=(VectorPAD& lhs, const VectorPAD& rhs)
	{
		lhs.P = clamp(lhs.P + rhs.P, -1.f, 1.f);
		lhs.A = clamp(lhs.A + rhs.A, -1.f, 1.f);
		lhs.D = clamp(lhs.D + rhs.D, -1.f, 1.f);
		return lhs;
	}

	inline VectorPAD operator+(const VectorPAD& lhs, const VectorPAD& rhs)
	{
		VectorPAD result = lhs;
		result += rhs;
		return result;
	}

	inline VectorPAD& operator-=(VectorPAD& lhs, const VectorPAD& rhs)
	{
		lhs.P = clamp(lhs.P - rhs.P, -1.f, 1.f);
		lhs.A = clamp(lhs.A - rhs.A, -1.f, 1.f);
		lhs.D = clamp(lhs.D - rhs.D, -1.f, 1.f);
		return lhs;
	}

	inline VectorPAD operator-(const VectorPAD& lhs, const VectorPAD& rhs)
	{
		VectorPAD result = lhs;
		result -= rhs;
		return result;
	}

	inline VectorPAD& operator*=(VectorPAD& lhs, float scalar)
	{
		lhs.P = clamp(lhs.P * scalar, -1.f, 1.f);
		lhs.A = clamp(lhs.A * scalar, -1.f, 1.f);
		lhs.D = clamp(lhs.D * scalar, -1.f, 1.f);
		return lhs;
	}

	inline VectorPAD operator*(const VectorPAD& lhs, float scalar)
	{
		VectorPAD result = lhs;
		result *= scalar;
		return result;
	}

	inline VectorPAD operator*(float scalar, const VectorPAD& vec)
	{
		return vec * scalar;
	}

	struct Matrix3x4
	{
		float M[3][4];
		Matrix3x4()
		{
			std::memset(&M[0], 0, sizeof(float) * 3 * 4);
		}
	};

	inline VectorPAD& operator*=(VectorPAD& lhs, const Matrix3x4& rhs)
	{
		float tempValues[3];
		for (auto i = 0; i < 3; ++i)
		{
			tempValues[i] = lhs._V[i] * rhs.M[i][0] + lhs._V[i] * rhs.M[i][1] + lhs._V[i] * rhs.M[i][2] + rhs.M[i][3];
			tempValues[i] = clamp(tempValues[i], -1.f, 1.f);
		}
		std::memcpy(&lhs._V[0], &tempValues[0], sizeof(float) * 3);
		return lhs;
	}

	inline VectorPAD operator*(const VectorPAD& lhs, const Matrix3x4& rhs)
	{
		VectorPAD result = lhs;
		result *= rhs;
		return result;
	}

	inline void VectorPAD::Normalize()
	{
		auto length = 1 / sqrt(LengthSquared());
		*this *= length;
	}

	enum class Mood
	{
		// + P + A + D Exuberant - P + A + D Enraged
		// + P + A - D Dependent - P + A - D Anxious
		// + P - A + D Relaxed   - P - A + D Disdainful
		// + P - A - D Docile    - P - A - D Bored
		Exuberant = 7,
		Dependent = 6,
		Relaxed = 5,
		Docile = 4,
		Enraged = 3,
		Anxious = 2,
		Disdainful = 1,
		Bored = 0,
	};

	struct Action
	{
		unsigned Id;
		VectorPAD Effect;
		unsigned SourceAgentId;
		unsigned TargetAgentId;
	};

	inline bool operator==(const Action& lhs, const Action& rhs)
	{
		return lhs.Id == rhs.Id && lhs.SourceAgentId == rhs.SourceAgentId && lhs.Effect == rhs.Effect && lhs.TargetAgentId == rhs.TargetAgentId;
	}

#define CPD_MAX_ACTIONS 16u
#define MAX_AGENT_ACTIONS 16u
	struct ActionDescription
	{
		unsigned Id;
		unsigned DependentActions[CPD_MAX_ACTIONS];
		size_t DependentActionsCount;
	};

	struct AgentDescription
	{
		ActionDescription Actions[MAX_AGENT_ACTIONS];
		size_t ActionCount;
	};
}