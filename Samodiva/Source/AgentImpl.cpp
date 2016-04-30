#include "precompiled.h"
#include "AgentImpl.h"

namespace Samodiva
{
static unsigned long g_IdCounter = 0;

struct EmotionalVector
{
	float Pleasure;
	float Arousal;
	float Dominance;
};

// Events extert emotional force
// The emotional acceleration is just like in newton's mechanics - a = F / I where I is the emotional inertia of agent - a matrix describing the agent's reactiveness to the stimuli
// Emotional Velocity = integral over acceleration ( the current active emotion), decays over time due to 'emotional gravity' towards neutrality and boredom
// Emotional state = integral over emotional velocity (the current mood)


AgentImpl::AgentImpl(const stl::string& agentClass)
	: m_Id(g_IdCounter++)
	, m_Class(agentClass)
{
}

void AgentImpl::Destroy()
{
	delete this;
}

unsigned long AgentImpl::GetId() const
{
	return m_Id;
}

const char* AgentImpl::GetClass() const
{
	return m_Class.c_str();
}

void AgentImpl::Update(float delta)
{
	m_EmotionalComponent.Update(delta);
}
Mood AgentImpl::GetMood() const
{
	return m_EmotionalComponent.GetMood();
}
void AgentImpl::Notify(const Action& action)
{
	m_EmotionalComponent.Notify(action);
}

}