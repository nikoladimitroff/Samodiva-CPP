#pragma once

#include <Samodiva/Config.h>
#include <Samodiva/EmotionalTypes.h>
#include "BayesianNetwork.h"

#define EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS 3u

namespace Samodiva
{
	struct ExpectedAction
	{
		float Expectation;
		Action ActionInstance;
	};
	inline bool operator==(const ExpectedAction& lhs, const ExpectedAction& rhs)
	{
		return EpsilonEquals(lhs.Expectation, rhs.Expectation) && lhs.ActionInstance == rhs.ActionInstance;
	}
	inline bool operator==(const ExpectedAction& lhs, const Action& rhs)
	{
		return lhs.ActionInstance == rhs;
	}

	class EmotionalComponent
	{
	public:
		EmotionalComponent();
		void Update(float);
		Mood GetMood() const;
		void Notify(const Action& action);

	private:
		VectorPAD ComputeActiveForce();

		VectorPAD m_Mood;
		VectorPAD m_ActiveEmotion;
		VectorPAD m_CurrentForce;
		Matrix3x4 m_EmotionalTensor;

		struct InteractingAgent
		{
			InteractingAgent(const stl::vector<ObservedAction>& actions)
				: Distribution(actions)
			{
			}
			unsigned Id;
			BayesianNetwork Distribution;
			Action PreviousActions[EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS];
		};
		stl::vector<InteractingAgent> m_OtherAgents;
		stl::vector<ExpectedAction> m_ExpectedActions;

		void CheckExpectations(const Action& action);
	};
}