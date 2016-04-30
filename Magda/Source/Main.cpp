#include <iostream>
#include <memory>
#include <string>

#include <Samodiva/World.h>
#include <Samodiva/Agent.h>
#include <Samodiva/EmotionalTypes.h>

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
	world_ptr world(CreateSamodivaWorld());
	world->LoadDirectory("airesources\\*");

	agent_ptr agent(world->CreateAgent("Test"));

	agent_ptr user(world->CreateAgent("User"));

	bool shouldExit = false;
	int counter = 0;
	do
	{
		agent->Update(1);
		std::cout << MoodToName(agent->GetMood()) << std::endl;

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

	//int x;
	//std::cin >> x;


	return 0;
}