#pragma once

#include <vector>
#include <Samodiva/EmotionalTypes.h>

namespace Samodiva
{

// Think of action variable as single bits in the unsigned index in the table
struct ObservedAction
{
	ObservedAction(unsigned id, const unsigned* actionIds, size_t actionCount);
	friend class BayesianNetwork;
private:
	unsigned m_Id;
	unsigned m_Occurences;

	unsigned m_DependentActionIds[CPD_MAX_ACTIONS];
	unsigned m_DependentActionOccurrences[CPD_MAX_ACTIONS];
	size_t m_DependentActionCount;
};

class BayesianNetwork
{
public:
	BayesianNetwork(const stl::vector<ObservedAction>& actions);

	float Infer(unsigned actionId, unsigned* evidenceActions, size_t evidenceActionsCount);
	void Update(unsigned actionId, unsigned* evidenceActions, size_t evidenceActionsCount);

private:
	stl::vector<ObservedAction> m_Distribution;
	size_t m_ObservedEventsCount;
};

}