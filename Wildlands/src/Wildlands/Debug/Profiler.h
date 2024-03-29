#pragma once
#include "Wildlands/Core/Base.h"

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <thread>

namespace Wildlands
{
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string Name;
		FloatingPointMicroseconds Start;
		std::chrono::microseconds ElapsedTime;
		std::thread::id ThreadID;
	};

	struct ProfilerSession
	{
		std::string Name;
	};

	class Profiler
	{
	public:
		Profiler()
			: m_CurrentSession(nullptr)
		{
		}

		static Profiler& Get()
		{
			static Profiler instance;
			return instance;
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession)
			{
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Log::GetCoreLogger())
				{ // Edge case: BeginSession() might be before Log::Init()
					WL_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
				}
				InternalEndSession();
			}
			m_OutputStream.open(filepath);
			if (m_OutputStream.is_open())
			{
				m_CurrentSession = new ProfilerSession({ name });
				WriteHeader();
			}
			else
			{
				if (Log::GetCoreLogger())
				{ // Edge case: BeginSession() might be before Log::Init()
					WL_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
				}
			}
		}

		void EndSession()
		{
			std::lock_guard lock(m_Mutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult& result)
		{
			std::stringstream json;
			
			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
			json << "\"name\":\"" << name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.ThreadID << ",";
			json << "\"ts\":" << result.Start.count();
			json << "}";

			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession)
			{
				m_OutputStream << json.str();
				m_OutputStream.flush();
			}
		}

	private:
		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		/// <summary>
		/// must own lock on m_Mutex before calling this function
		/// </summary>
		void InternalEndSession()
		{
			if (m_CurrentSession)
			{
				WriteFooter();
				m_OutputStream.close();
				delete m_CurrentSession;
				m_CurrentSession = nullptr;
			}
		}

	private:
		std::mutex m_Mutex;
		ProfilerSession* m_CurrentSession;
		std::ofstream m_OutputStream;
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char* name)
			: m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::steady_clock::now();
		}

		~ProfilerTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::steady_clock::now();
			auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
			auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch()
				         - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

			Profiler::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

			m_Stopped = true;
		}
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
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
