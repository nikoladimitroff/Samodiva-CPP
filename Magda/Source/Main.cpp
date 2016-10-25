#include <iostream>
#include <memory>
#include <string>
#include <Windows.h>

#include <Samodiva/World.h>
#include <Samodiva/Agent.h>
#include <Samodiva/Allocator.h>
#include <Samodiva/LogHandler.h>
#include <Samodiva/EmotionalTypes.h>

class StdLogger : public Samodiva::ILogHandler
{
public:
	const char* StringifySeverity(Samodiva::ILogHandler::LogSeverity severity)
	{
		switch (severity)
		{
		case Samodiva::ILogHandler::LS_Debug: return "Debug";
		case Samodiva::ILogHandler::LS_Trace: return "Trace";
		case Samodiva::ILogHandler::LS_Info: return "Info";
		case Samodiva::ILogHandler::LS_Warning: return "Warning";
		case Samodiva::ILogHandler::LS_Error: return "Error";
		default:
			return "Unknown severity";
		}
	}

	virtual void WriteLog(LogSeverity severity, const char* message)
	{
		printf("Samodiva [%s]: %s", StringifySeverity(severity), message);
	}
};

class MallocAllocator : public Samodiva::IAllocator
{
public:
	virtual void* Malloc(size_t size, unsigned alignment)
	{
		auto ptr = std::malloc(size);
		auto Log = &::OutputDebugStringA;
		Log("Allocating ");
		Log(std::to_string(size).c_str());
		Log(" bytes at address ");
		Log(std::to_string((unsigned long)ptr).c_str());
		Log("\r\n");
		return ptr;
	}
	virtual void Free(void* ptr)
	{
		std::free(ptr);
		auto Log = &::OutputDebugStringA;
		Log("Freeing ");
		Log(std::to_string((unsigned long)ptr).c_str());
		Log("\r\n");
	}
	virtual void* Realloc(void* ptr, size_t newSize)
	{
		return std::realloc(ptr, newSize);
	}
};

template<typename T>
class DestroyDeleter
{
public:
	void operator()(T* ptr)
	{
		ptr->Destroy();
	}
};

typedef std::unique_ptr<Samodiva::World, DestroyDeleter<Samodiva::World>> world_ptr;
typedef std::unique_ptr<Samodiva::Agent, DestroyDeleter<Samodiva::Agent>> agent_ptr;

std::string MoodToName(Samodiva::Mood mood)
{
	switch (mood)
	{
	case Samodiva::Mood::Exuberant: return "Exuberant";
	case Samodiva::Mood::Dependent: return "Dependent";
	case Samodiva::Mood::Relaxed: return "Relaxed";
	case Samodiva::Mood::Docile: return "Docile";
	case Samodiva::Mood::Enraged: return "Enraged";
	case Samodiva::Mood::Anxious: return "Anxious";
	case Samodiva::Mood::Disdainful: return "Disdainful";
	case Samodiva::Mood::Bored: return "Bored";
	default: return "UNKNOWN";
	}
}

int main()
{
	Samodiva::WorldSettings settings;
	StdLogger logger;
	settings.LogHandler = &logger;
	MallocAllocator allocator;
	settings.Allocator = &allocator;
	world_ptr world(CreateSamodivaWorld(settings));
	world->LoadDirectory("airesources\\*");

	agent_ptr agent(world->CreateAgent("Test"));

	agent_ptr user(world->CreateAgent("User"));

	bool shouldExit = false;
	int counter = 0;
	do
	{
		agent->Update(1);
		//std::cout << MoodToName(agent->GetMood()) << std::endl;

		//std::cout << "Enter action: ";
		//std::string actionName;
		//std::cin >> actionName;
		//if (actionName == "quit")
		//{
		//	shouldExit = true;
		//}
		//if (actionName == "skip")
		//{
		//	continue;
		//}
		//else
		{
			Samodiva::Action action = world->InstantiateAction("pet");
			action.SourceAgentId = user->GetId();
			action.TargetAgentId = agent->GetId();
			agent->Notify(action);
		}
		counter++;
		shouldExit = counter == 500;
	}
	while (!shouldExit);

	int x;
	std::cin >> x;


	return 0;
}