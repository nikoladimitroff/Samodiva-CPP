#pragma once
#include <string>
#include <stdio.h>
#include <Samodiva/LogHandler.h>

namespace Samodiva
{
extern Samodiva::ILogHandler* g_LogHandler;
}

#define SAMODIVA_LOG(Severity, Message, ...) \
	{ \
		TempString buffer(sizeof(Message) * 2, '\0'); \
		sprintf_s(&buffer[0], sizeof(Message) * 2, Message, __VA_ARGS__); \
		g_LogHandler->WriteLog(Samodiva::ILogHandler::LS_Info, buffer.c_str()); \
	}