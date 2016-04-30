#include "precompiled.h"
#include "BayesianNetwork.h"

#include <iterator>
#include <algorithm>

namespace Samodiva
{
	static const unsigned INITIAL_EVENT_COUNT = 0u;
	ObservedAction::ObservedAction(unsigned id, const unsigned* actionIds, size_t actionCount)
		: m_Id(id)
		, m_Occurences(INITIAL_EVENT_COUNT)
		, m_DependentActionCount(actionCount)
	{
		std::memcpy(&m_DependentActionIds[0], actionIds, actionCount * sizeof(unsigned));
		std::fill(&m_DependentActionOccurrences[0], &m_DependentActionOccurrences[0] + (2 << actionCount), INITIAL_EVENT_COUNT);
	}

	BayesianNetwork::BayesianNetwork(const stl::vector<ObservedAction>& actions)
		: m_ObservedEventsCount(INITIAL_EVENT_COUNT)
		, m_Distribution(actions)
	{
	}

	float BayesianNetwork::Infer(unsigned actionId, unsigned* evidenceActions, size_t evidenceActionsCount)
	{
		auto existingAction = std::find_if(m_Distribution.begin(), m_Distribution.end(), [actionId](auto action)
		{
			return action.m_Id == actionId;
		});
		assert(existingAction != m_Distribution.end());
		if (existingAction->m_DependentActionCount == 0)
		{
			auto prior = existingAction->m_Occurences / static_cast<float>(m_ObservedEventsCount);
			return prior;
		}
		if (existingAction->m_Occurences == 0)
		{
			return 0;
		}

		unsigned evidenceIndex = 0;
		for (auto i = 0; i < evidenceActionsCount; ++i)
		{
			for (auto j = 0; j < existingAction->m_DependentActionCount; ++j)
			{
				if (existingAction->m_DependentActionIds[j] == evidenceActions[i])
				{
					evidenceIndex |= 1 << j;
				}
			}
		}
		auto posterior = existingAction->m_DependentActionOccurrences[evidenceIndex] / static_cast<float>(existingAction->m_Occurences);
		return posterior;
	}

	void BayesianNetwork::Update(unsigned actionId, unsigned* evidenceActions, size_t evidenceActionsCount)
	{
		auto existingAction = std::find_if(m_Distribution.begin(), m_Distribution.end(), [actionId](auto action)
		{
			return action.m_Id == actionId;
		});
		assert(existingAction != m_Distribution.end());
		++existingAction->m_Occurences;
		++m_ObservedEventsCount;
		unsigned evidenceIndex = 0;
		for (auto i = 0; i < evidenceActionsCount; ++i)
		{
			for (auto j = 0; j < existingAction->m_DependentActionCount; ++j)
			{
				if (existingAction->m_DependentActionIds[j] == evidenceActions[i])
				{
					evidenceIndex |= 1 << j;
				}
			}
		}
		++existingAction->m_DependentActionOccurrences[evidenceIndex];
	}

}
