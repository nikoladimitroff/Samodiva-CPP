#pragma once

#include <Samodiva/Agent.h>
#include "EmotionalComponent.h"

namespace Samodiva
{

class AgentImpl : public Agent
{
public:
	AgentImpl(const stl::string& agentClass);

	virtual void Destroy() override;

	virtual unsigned long GetId() const override;
	virtual const char* GetClass() const override;

	virtual void Update(float delta) override;
	// Emotional interface
	virtual Mood GetMood() const override;
	virtual void Notify(const Action& action) override;
private:
	unsigned long m_Id;
	stl::string m_Class;
	EmotionalComponent m_EmotionalComponent;
};

}