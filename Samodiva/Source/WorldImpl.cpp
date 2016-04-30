#include "precompiled.h"
#include "WorldImpl.h"

#include "AgentImpl.h"


namespace Samodiva
{
WorldImpl* WorldImpl::s_Instance = nullptr;

Agent* WorldImpl::CreateAgent(const char* agentClass)
{
	auto description = InstantiateAgentDescription(agentClass);
	auto agent = new AgentImpl(agentClass);
	m_Agents[agent->GetId()] = agent;
	return agent;
}

void WorldImpl::Destroy()
{
	delete this;
}

Action WorldImpl::InstantiateAction(const char* actionName) const
{
	return m_Librarian.InstantiateAction(actionName);
}
Action WorldImpl::InstantiateAction(unsigned actionId) const
{
	return m_Librarian.InstantiateAction(actionId);
}
AgentDescription WorldImpl::InstantiateAgentDescription(const char* agentName) const
{
	return m_Librarian.InstantiateAgentDescription(agentName);
}

void WorldImpl::LoadFile(const char* path)
{
	m_Librarian.LoadFile(path);
}

void WorldImpl::LoadDirectory(const char* path)
{
	m_Librarian.LoadDirectory(path);
}

}

Samodiva::World* CreateSamodivaWorld()
{
	return new Samodiva::WorldImpl();
}