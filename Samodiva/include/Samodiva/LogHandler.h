#pragma once

namespace Samodiva
{
	class ILogHandler
	{
	public:
		virtual ~ILogHandler() {}
		enum LogSeverity
		{
			LS_Debug,
			LS_Trace,
			LS_Info,
			LS_Warning,
			LS_Error
		};
		virtual void WriteLog(LogSeverity severity, const char* message) = 0;
	};
}