#pragma once
#include <functional>

#include "../MemoryManagement/MemoryManagement.h"

namespace Samodiva
{

template<typename FuzzyClassType, typename InputData>
struct FuzzyVariable
{
	static_assert(std::is_enum<FuzzyClassType>::value, "FuzzyClassType must be an enumerated type!");

	FuzzyClassType Type;
	std::function<float(const InputData&)> MembershipFunction;
};

template<typename FuzzyClassesType, int FuzzyClassCount, typename InputData>
class Fuzzifier
{
public:
	using VariableType = FuzzyVariable<FuzzyClassesType, InputData>;
	Fuzzifier(VariableType classes[FuzzyClassCount]);

	float ComputeMembershipFor(FuzzyClassesType type, const InputData& input);
	tmp::vector<float> ComputeMembershipForAll(const InputData& input);
private:
	VariableType m_Classes[FuzzyClassCount];
};

}