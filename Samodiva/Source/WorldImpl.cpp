#include "precompiled.h"
#include "WorldImpl.h"

#include "AgentImpl.h"
#include "TaskSystem/TaskSystem.h"

namespace Samodiva
{
WorldImpl* WorldImpl::s_Instance = nullptr;
ILogHandler* g_LogHandler = nullptr;

inline int GetThreadId()
{
	return static_cast<int>(std::hash<std::thread::id>()(std::this_thread::get_id()));
}

void WorldImpl::Initialize(const WorldSettings&)
{
	TaskSystem<3> taskSystem;
	taskSystem.SpawnTask("Dulila0", 0, []() { SAMODIVA_FORMAT_LOG(Info, "dulila @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila0", 0, []() { SAMODIVA_FORMAT_LOG(Trace, "dulila2 @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Foodida1", 1, []() { SAMODIVA_FORMAT_LOG(Info, "Foodida @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila1", 1, []() { SAMODIVA_FORMAT_LOG(Error, "dulila3 @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila0", 0, []() { SAMODIVA_FORMAT_LOG(Info, "dulila4 @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila2", 2, []() { SAMODIVA_FORMAT_LOG(Warning, "dulila5 @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila2", 2, []() { SAMODIVA_FORMAT_LOG(Debug, "dulila6 @ %d \r\n", GetThreadId()); });
	taskSystem.SpawnTask("Dulila0", 0, []() { SAMODIVA_FORMAT_LOG(Info, "dulila7 @ %d \r\n", GetThreadId()); });
}

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

Samodiva::World* CreateSamodivaWorld(const Samodiva::WorldSettings& settings)
{
	// Initialize global subsystems:
	Samodiva::g_LogHandler = settings.LogHandler;
	Samodiva::g_Allocator = settings.Allocator;

	auto world = new Samodiva::WorldImpl();
	world->Initialize(settings);
	return world;
}