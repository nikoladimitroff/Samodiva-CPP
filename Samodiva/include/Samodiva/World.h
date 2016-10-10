#pragma once

#include <Samodiva/config.h>
#include <Samodiva/EmotionalTypes.h>

namespace Samodiva
{

class Agent;

struct WorldSettings
{
	WorldSettings::WorldSettings()
		: LogHandler(nullptr)
	{}

	class ILogHandler* LogHandler;
};

class World
{
public:
	virtual Agent* CreateAgent(const char* agentClass) = 0;

	virtual Action InstantiateAction(const char* actionName) const = 0;
	virtual Action InstantiateAction(unsigned actionId) const = 0;
	virtual AgentDescription InstantiateAgentDescription(const char* agentName) const = 0;

	virtual void LoadFile(const char* path) = 0;
	virtual void LoadDirectory(const char* path) = 0;

	virtual void Destroy() = 0;
};

}

extern "C" SAMODIVA_API Samodiva::World* CreateSamodivaWorld(const Samodiva::WorldSettings& settings);