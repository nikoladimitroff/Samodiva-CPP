#include "precompiled.h"
#include "WorldImpl.h"

#include "AgentImpl.h"
#include "TaskSystem/TaskSystem.h"

namespace Samodiva
{
WorldImpl* WorldImpl::s_Instance = nullptr;

void WorldImpl::Initialize()
{
	TaskSystem<3> taskSystem;
	taskSystem.SpawnTask("Dulila0", 0, []() { std::cout << "dulila @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila0", 0, []() { std::cout << "dulila2 @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Foodida1", 1, []() { std::cout << "Foodida @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila1", 1, []() { std::cout << "dulila3 @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila0", 0, []() { std::cout << "dulila4 @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila2", 2, []() { std::cout << "dulila5 @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila2", 2, []() { std::cout << "dulila6 @ " << std::this_thread::get_id << "\r\n"; });
	taskSystem.SpawnTask("Dulila0", 0, []() { std::cout << "dulila7 @ " << std::this_thread::get_id << "\r\n"; });
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

Samodiva::World* CreateSamodivaWorld()
{
	auto world = new Samodiva::WorldImpl();
	world->Initialize();
	return world;
}