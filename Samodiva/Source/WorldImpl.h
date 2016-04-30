#pragma once

#include <Samodiva/World.h>
#include <Samodiva/EmotionalTypes.h>

#include "Librarian.h"

namespace Samodiva
{

class WorldImpl : public World
{
public:
	WorldImpl()
	{
		s_Instance = this;
	}
	virtual Agent* CreateAgent(const char* agentClass) override;
	virtual void Destroy() override;

	virtual Action InstantiateAction(const char* actionName) const override;
	virtual Action InstantiateAction(unsigned actionId) const override;
	virtual AgentDescription InstantiateAgentDescription(const char* agentName) const override;

	virtual void LoadFile(const char* path) override;
	virtual void LoadDirectory(const char* path) override;

	inline static WorldImpl* GetInstance() { return s_Instance; }
	Agent* FindAgent(unsigned id) { return m_Agents[id]; } // TODO: handle nonexisting
	stl::string GetActionName(unsigned id) { return m_Librarian.GetActionName(id); }

private:
	Librarian m_Librarian;
	std::unordered_map<unsigned, Agent*> m_Agents;

	static WorldImpl* s_Instance;
};

}