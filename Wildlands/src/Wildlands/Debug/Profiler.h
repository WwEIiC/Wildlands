#pragma once
#include "Wildlands/Core/Core.h"

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Wildlands
{
	struct ProfileResult
	{
		std::string Name;
		long long Start, End;
		uint32_t ThreadID;
	};

	struct ProfilerSession
	{
		std::string Name;
	};

	class Profiler
	{
	public:
		Profiler()
			: m_CurrentSession(nullptr), m_ProfileCount(0)
		{
		}

		static Profiler& Get()
		{
			static Profiler instance;
			return instance;
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			m_OutputStream.open(filepath);
			WriteHeader();
			m_CurrentSession = new ProfilerSession{ name };
		}

		void EndSession()
		{
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
			m_ProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (m_ProfileCount++ > 0)
				m_OutputStream << ",";

			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"tid\":" << result.ThreadID << ",";
			m_OutputStream << "\"ts\":" << result.Start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

	private:
		ProfilerSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		int m_ProfileCount;
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char* name)
			: m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~ProfilerTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Profiler::Get().WriteProfile({ m_Name, start, end, threadID });

			m_Stopped = true;
		}
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}

#define WL_PROFILE 1
#if WL_PROFILE
#define WL_PROFILE_BEGIN_SESSION(name, filepath) ::Wildlands::Profiler::Get().BeginSession(name, filepath)
#define WL_PROFILE_END_SESSION() ::Wildlands::Profiler::Get().EndSession()
#define WL_PROFILE_SCOPE(name) ::Wildlands::ProfilerTimer CONNECT(timer,__LINE__)(name);
#define WL_PROFILE_FUNCTION() WL_PROFILE_SCOPE(__FUNCSIG__)
#else
#define WL_PROFILE_BEGIN_SESSION(name, filepath)
#define WL_PROFILE_END_SESSION()
#define WL_PROFILE_SCOPE(name)
#define WL_PROFILE_FUNCTION()
#endif
