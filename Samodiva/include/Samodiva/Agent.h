#pragma once

#include "Config.h"
#include <Samodiva/EmotionalTypes.h>

namespace Samodiva
{
	class Agent
	{
	public:
		virtual void Destroy() = 0;
		virtual unsigned long GetId() const = 0;
		virtual const char* GetClass() const = 0;

		virtual void Update(float delta) = 0;
		// Emotional interface
		virtual Mood GetMood() const = 0;
		virtual void Notify(const Action& action) = 0;
	};

}