#pragma once
#include <string>
#include <stdio.h>
#include <Samodiva/LogHandler.h>

namespace Samodiva
{
extern Samodiva::ILogHandler* g_LogHandler;
}

#define SAMODIVA_LOG(Severity, Message) \
	do \
	{ \
		g_LogHandler->WriteLog(Samodiva::ILogHandler::LS_##Severity, Message); \
	} \
	while(0, 0)

#define SAMODIVA_FORMAT_LOG(Severity, Message, ...) \
	do \
	{ \
		tmp::string buffer(sizeof(Message) * 2, '\0'); \
		sprintf_s(&buffer[0], sizeof(Message) * 2, Message, __VA_ARGS__); \
		g_LogHandler->WriteLog(Samodiva::ILogHandler::LS_##Severity, buffer.c_str()); \
	}\
	while(0, 0)
