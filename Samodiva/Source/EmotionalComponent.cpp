#include "precompiled.h"
#include "EmotionalComponent.h"

#include <Samodiva/Agent.h>
#include "WorldImpl.h"
#include "MathHelpers.h"

#include <iostream>

static const float ACTIVATION_EXPECTATION_THRESHOLD = 0.65f;

namespace Samodiva
{
	EmotionalComponent::EmotionalComponent()
	{
		for (auto i = 0; i < 3; ++i)
		{
			m_EmotionalTensor.M[i][i] = 1.f;
		}
	}

	void EmotionalComponent::Update(float delta)
	{
		// Update expectations
		m_ExpectedActions.clear();
		//stl::cout << "EXPECTATIONS: " << stl::endl;
		for (auto& partner : m_OtherAgents)
		{
			auto description = WorldImpl::GetInstance()->InstantiateAgentDescription(WorldImpl::GetInstance()->FindAgent(partner.Id)->GetClass());
			for (auto i = 0; i < description.ActionCount; ++i)
			{
				unsigned previousActionIndices[EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS];
				for (auto j = 0; j < EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS; ++j)
				{
					previousActionIndices[j] = partner.PreviousActions[j].Id;
				}
				auto expectation = partner.Distribution.Infer(description.Actions[i].Id, &previousActionIndices[0], EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS);
				//std::cout << WorldImpl::GetInstance()->GetActionName(description.Actions[i].Id) << " | " << expectation << stl::endl;
				if (expectation >= ACTIVATION_EXPECTATION_THRESHOLD)
				{
					// TODO: Consider event relevance, impact, source and target
					Action expectedAction = WorldImpl::GetInstance()->InstantiateAction(description.Actions[i].Id);
					expectedAction.SourceAgentId = partner.Id;
					expectedAction.TargetAgentId = static_cast<unsigned>(-1);
					ExpectedAction expected;
					expected.Expectation = expectation;
					expected.ActionInstance = expectedAction;
					m_ExpectedActions.push_back(expected);
				}
			}
		}
		//stl::cout << "---: " << stl::endl;

		// TODO: Implement RK4
		auto acceleration = ComputeActiveForce() * m_EmotionalTensor;
		m_ActiveEmotion += acceleration * delta;
		m_Mood += m_ActiveEmotion * delta;

		//stl::cout << "Active emotion: (" << m_ActiveEmotion.P << ", " << m_ActiveEmotion.A << ", " << m_ActiveEmotion.D << ")" << stl::endl;
		//stl::cout << "Mood emotion: (" << m_Mood.P << ", " << m_Mood.A << ", " << m_Mood.D << ")" << stl::endl;
		m_CurrentForce = VectorPAD();
	}

	VectorPAD EmotionalComponent::ComputeActiveForce()
	{
		VectorPAD cognitiveForce;
		for (auto& expected : m_ExpectedActions)
		{
			auto& action = expected.ActionInstance;
			if (action.Effect.P * action.Effect.D < 0 || (action.Effect.P == 0 && action.Effect.D == 0))
			{
				//stl::cout << "EXPECTING action " << WorldImpl::GetInstance()->GetActionName(action.Id) << stl::endl;
			}
			else if (action.Effect.P <= 0 && action.Effect.D <= 0)
			{
				//stl::cout << "FEARING action " << WorldImpl::GetInstance()->GetActionName(action.Id) << stl::endl;
				cognitiveForce -= action.Effect * expected.Expectation;
				cognitiveForce += VectorPAD(0, sqrt(expected.Expectation), 0);
			}
			else
			{
				//stl::cout << "HOPING FOR action " << WorldImpl::GetInstance()->GetActionName(action.Id) << stl::endl;
				cognitiveForce += action.Effect * expected.Expectation;
				cognitiveForce += VectorPAD(0, sqrt(expected.Expectation), 0);
			}
		}
		VectorPAD friction = VectorPAD(-1.f, -1.f, -1.f) - m_Mood;
		friction.Normalize();
		static const float DEFAULT_ACCELERATION = 0.08f;
		friction *= DEFAULT_ACCELERATION;

		return m_CurrentForce + cognitiveForce + friction;
	}

	Mood EmotionalComponent::GetMood() const
	{
		// + P + A + D Exuberant - P + A + D Enraged
		// + P + A - D Dependent - P + A - D Anxious
		// + P - A + D Relaxed   - P - A + D Disdainful
		// + P - A - D Docile    - P - A - D Bored

		// Get the signs of the vector in PAD space, convert to a 3-bit number and cast that to mood)
		auto result = nonnegative_sign(m_Mood.P) << 2 |
				nonnegative_sign(m_Mood.A) << 1 |
				nonnegative_sign(m_Mood.D);

		return (Mood)result;
	}

	void EmotionalComponent::Notify(const Action& action)
	{
		m_CurrentForce += action.Effect;
		auto agent = std::find_if(m_OtherAgents.begin(), m_OtherAgents.end(), [&action](const auto& agent)
		{
			return agent.Id == action.SourceAgentId;
		});
		InteractingAgent* interactingAgentPtr = nullptr;
		if (agent == m_OtherAgents.end())
		{
			// TODO: check if we should interact with this agent
			auto newAgent = WorldImpl::GetInstance()->FindAgent(action.SourceAgentId);
			auto description = WorldImpl::GetInstance()->InstantiateAgentDescription(newAgent->GetClass());
			stl::vector<ObservedAction> actions;
			for (auto i = 0; i < description.ActionCount; ++i)
			{
				actions.push_back(ObservedAction(description.Actions[i].Id, description.Actions[i].DependentActions, description.Actions[i].DependentActionsCount));
			}
			InteractingAgent newPartner(actions);
			newPartner.Id = action.SourceAgentId;
			m_OtherAgents.push_back(newPartner);
			interactingAgentPtr = &m_OtherAgents[m_OtherAgents.size() - 1];
		}
		if (interactingAgentPtr == nullptr)
		{
			interactingAgentPtr = &*agent;
		}
		CheckExpectations(action);
		unsigned previousActionIndices[EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS];
		for (auto i = 0; i < EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS; ++i)
		{
			previousActionIndices[i] = interactingAgentPtr->PreviousActions[i].Id;
		}
		interactingAgentPtr->Distribution.Update(action.Id, previousActionIndices, EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS);
		std::rotate(interactingAgentPtr->PreviousActions, interactingAgentPtr->PreviousActions + 1, interactingAgentPtr->PreviousActions + EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS);
		interactingAgentPtr->PreviousActions[EMOTIONAL_COMPONENT_MAX_TRACKED_ACTIONS - 1] = action;
	}

	void EmotionalComponent::CheckExpectations(const Action& action)
	{
		// Check if it's suprising
		if (action.Effect.LengthSquared() && std::find(m_ExpectedActions.cbegin(), m_ExpectedActions.cend(), action) != m_ExpectedActions.cend())
		{
			std::cout << "SUPRISE! Action " << action.Id << " was unexpected" << std::endl;
			m_CurrentForce += VectorPAD(0, sqrt(action.Effect.LengthSquared()), 0);
		}
	}
}