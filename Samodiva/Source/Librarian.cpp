#include "precompiled.h"
#include "Librarian.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <fstream>
#include <regex>

#ifdef SAMODIVA_PLATFORM_WIN
#include <Windows.h>
#endif

namespace Samodiva {

Librarian::Librarian()
{
}


Librarian::~Librarian()
{
}

namespace
{
	template<typename T>
	T JsonTo(const JSON& data)
	{
		// Should not be called
		assert(false);
	}
}

void Librarian::LoadFile(const char* path)
{
	std::ifstream file(path);
	if (!file)
	{
		// TODO: LOG ERROR
		assert(false);
	}
	auto parsedData = JSON::parse(file);
	auto fileType = parsedData["header"].get<std::string>(); // the json library only supports std
	if (fileType.compare("Action Library") == 0)
	{
		LoadAction(parsedData);
	}
	if (fileType.compare("Agent Description") == 0)
	{
		LoadAgent(parsedData);
	}
}

void Librarian::LoadAction(const JSON& data)
{
	auto& actions = data["actions"];
	for (auto& actionDescription : actions)
	{
		stl::string name(actionDescription["name"].get<std::string>().c_str()); // the json library only supports std
		VectorPAD effect(actionDescription["effect"][0],
			actionDescription["effect"][1],
			actionDescription["effect"][2]);
		Action actionData;
		actionData.Id = static_cast<unsigned>(m_ActionLibrary.size());
		actionData.Effect = effect;
		m_ActionLibrary.push_back(std::make_pair(std::move(name), actionData));
	}
}


Samodiva::Mood NameToMood(const tmp::string& emotionName)
{
	if (emotionName == "Exuberant") return Samodiva::Mood::Exuberant;
	if (emotionName == "Dependent") return Samodiva::Mood::Dependent;
	if (emotionName == "Relaxed") return Samodiva::Mood::Relaxed;
	if (emotionName == "Docile") return Samodiva::Mood::Docile;
	if (emotionName == "Enraged") return Samodiva::Mood::Enraged;
	if (emotionName == "Anxious") return Samodiva::Mood::Anxious;
	if (emotionName == "Disdainful") return Samodiva::Mood::Disdainful;
	if (emotionName == "Bored") return Samodiva::Mood::Bored;
	assert(false);
	return Samodiva::Mood::Exuberant;
}

DecisionRule::QuantityModifier NameToQuantityModifier(const tmp::string& name)
{
	if (name == "not") return DecisionRule::QuantityModifier::Not;
	if (name == "almost") return DecisionRule::QuantityModifier::Almost;
	if (name == "mildly") return DecisionRule::QuantityModifier::Mildly;
	if (name == "very") return DecisionRule::QuantityModifier::Very;
	assert(false);
	return DecisionRule::QuantityModifier::None;
}


void Librarian::LoadAgent(const JSON & data)
{
	AgentDescription agentDescription = { 0 };
	// Parse actions
	auto& actions = data["actions"];
	for (auto& actionDescription : actions)
	{
		stl::string name(actionDescription["name"].get<std::string>().c_str());
		ActionDescription description = { 0 };
		auto position = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [&name](decltype(m_ActionLibrary)::value_type pair)
		{
			return pair.first == name;
		});
		assert(position != m_ActionLibrary.end());
		description.Id = position->second.Id;
		std::fill(&description.DependentActions[0], &description.DependentActions[0] + CPD_MAX_ACTIONS, static_cast<unsigned>(-1));
		for (auto& dependentAction : actionDescription["prior"])
		{
			stl::string actionName(dependentAction.get<std::string>().c_str());
			auto actionPosition = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [&actionName](decltype(m_ActionLibrary)::value_type pair)
			{
				return pair.first == actionName;
			});
			assert(actionPosition != m_ActionLibrary.end());
			description.DependentActions[description.DependentActionsCount++] = actionPosition->second.Id;
		}
		agentDescription.Actions[agentDescription.ActionCount++] = description;
	}
	// Parse decision making rules; swap to tmp::strings
	auto& rules = data["decisionmaking"];
	std::regex ruleRegex("If (self|player) is ((?:(?:not|almost|midly|very) ){0,4})(Exuberant|Dependent|Relaxed|Docile|Enraged|Anxious|Disdainful|Bored) Then (\\w| )*? (is|from|at|to) (self|player)");
	for (auto& ruleIterator : rules)
	{
		DecisionRule rule;
		int ruleCount = 0;
		std::smatch match;
		auto ruleAsText = ruleIterator.get<std::string>();
		std::regex_match(ruleAsText, match, ruleRegex);
		DecisionRule::Antecedent antecedent;
		antecedent.TargetAgent = DecisionRule::Target::Self;
		assert(match[1].compare("self") == 0);
		auto modifiers = match[2].str();
		if (modifiers.size() != 0) modifiers.pop_back(); // the whitespace at the end
		int modifiersCount = 0;
		for (size_t i = 0; modifiers.size() != 0 && i != std::string::npos; i = modifiers.find(' ', i + 1))
		{
			tmp::string modifier(modifiers.substr(i, modifiers.find(' ', i + 1)).c_str());
			antecedent.Modifiers[modifiersCount++] = NameToQuantityModifier(modifier);
		}
		antecedent.Emotion = NameToMood(tmp::string(match[3].str().c_str()));
		antecedent.NextAntecedentJoint = DecisionRule::Joint::None;
		rule.Antecedents[ruleCount++] = antecedent;

		auto actionName = match[4].str();
		auto actionPosition = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [&actionName](decltype(m_ActionLibrary)::value_type pair)
		{
			return pair.first == actionName.c_str() == 0;
		});
		assert(actionPosition != m_ActionLibrary.end());
		rule.ConsequentActionId = actionPosition->second.Id;
		rule.ConsequentActionTarget = DecisionRule::Target::Player;
		agentDescription.DecisionRules[agentDescription.DecisionRulesCount++] = rule;
	}
	stl::string agentClass(data["class"].get<std::string>().c_str());
	m_AgentLibrary.push_back(std::make_pair(std::move(agentClass), agentDescription));

}

void Librarian::LoadDirectory(const char* path)
{
#ifdef SAMODIVA_PLATFORM_WIN
	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(path, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (std::strcmp(data.cFileName, ".") * std::strcmp(data.cFileName, "..") != 0)
			{
				stl::string fullPath(path, std::strlen(path) - 1);
				fullPath.append(data.cFileName);
				LoadFile(fullPath.c_str());
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
#endif
}


Action Librarian::InstantiateAction(const char* actionName) const
{
	stl::string name(actionName);
	auto position = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [&name](decltype(m_ActionLibrary)::value_type pair)
	{
		return pair.first == name;
	});
	if (position == m_ActionLibrary.end())
	{
		assert(false);
		return Action();
	}
	return position->second;
}

Action Librarian::InstantiateAction(unsigned id) const
{
	auto position = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [id](decltype(m_ActionLibrary)::value_type pair)
	{
		return pair.second.Id == id;
	});
	if (position == m_ActionLibrary.end())
	{
		assert(false);
		return Action();
	}
	return position->second;
}

AgentDescription Librarian::InstantiateAgentDescription(const char* agentName) const
{
	stl::string name(agentName);
	auto position = std::find_if(m_AgentLibrary.cbegin(), m_AgentLibrary.cend(), [&name](decltype(m_AgentLibrary)::value_type pair)
	{
		return pair.first == name;
	});
	if (position == m_AgentLibrary.end())
	{
		assert(false);
	}
	return position->second;
}

stl::string Librarian::GetActionName(unsigned actionId) const
{
	auto position = std::find_if(m_ActionLibrary.begin(), m_ActionLibrary.end(), [actionId](decltype(m_ActionLibrary)::value_type pair)
	{
		return pair.second.Id == actionId;
	});
	if (position == m_ActionLibrary.end())
	{
		assert(false);
		return "";
	}
	return position->first;

}

}