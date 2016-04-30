#pragma once
#include <Samodiva/EmotionalTypes.h>

#pragma warning(push)
#pragma warning(disable: 4706)
#include <nlohmann/json.hpp>
#pragma warning(pop)

namespace Samodiva
{
	using JSON = nlohmann::json;
	class Librarian
	{
	public:
		Librarian();
		~Librarian();

		void LoadFile(const char* path);
		void LoadDirectory(const char* path);
		Action InstantiateAction(const char* actionName) const;
		Action InstantiateAction(unsigned actionId) const;
		AgentDescription InstantiateAgentDescription(const char* agentName) const;

		stl::string GetActionName(unsigned actionId) const;

	private:
		stl::vector<stl::pair<stl::string, Action>> m_ActionLibrary;
		stl::vector<stl::pair<stl::string, AgentDescription>> m_AgentLibrary;

		void LoadAction(const JSON& data);
		void LoadAgent(const JSON& data);
	};
}