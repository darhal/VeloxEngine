#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>
#include <Legacy/DataStructure/String/String.hpp>
#include <Legacy/Misc/Singleton/Singleton.hpp>
#include <Legacy/Utils/Logging.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>

#if defined(DEBUG)
#define PROFILE
#else
#define PROFILE
#endif

TRE_NS_START

struct ProfileResult
{
	String Name;

	std::chrono::duration<double, std::micro> Start;
	std::chrono::microseconds ElapsedTime;
	std::thread::id ThreadID;
};

struct ProfileSession
{
	String Name;
};

class Profiler : public Singleton<Profiler>
{
private:
	std::mutex m_Mutex;
	ProfileSession* m_CurrentSession;
	std::ofstream m_OutputStream;
public:
	Profiler()
		: m_CurrentSession(nullptr)
	{
	}

	void BeginSession(const String& name, const String& filepath = "results.json")
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (m_CurrentSession) {
			// If there is already a current session, then close it before beginning new one.
			// Subsequent profiling output meant for the original session will end up in the
			// newly opened session instead.  That's better than having badly formatted
			// profiling output.
			Log::Write(Log::WARN, "Instrumentor::BeginSession('%s') when session '%s' already open.", name.Buffer(), m_CurrentSession->Name.Buffer());
			InternalEndSession();
		}

		m_OutputStream.open(filepath.Buffer());

		if (m_OutputStream.is_open()) {
			m_CurrentSession = new ProfileSession({ name });
			WriteHeader();
		} else {
			Log::Write(Log::ERR, "Instrumentor could not open results file '%s'.", filepath.Buffer());
		}
	}

	void EndSession()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		InternalEndSession();
	}

	void WriteProfile(const ProfileResult& result)
	{
		std::stringstream json;

		std::string name = result.Name.Buffer();
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

		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_CurrentSession) {
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

	// Note: you must already own lock on m_Mutex before
	// calling InternalEndSession()
	void InternalEndSession()
	{
		if (m_CurrentSession) {
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
		}
	}

};

struct ProfileTimer
{
public:
	ProfileTimer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::steady_clock::now();
	}

	~ProfileTimer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::steady_clock::now();
		auto highResStart = std::chrono::duration<double, std::micro>{ m_StartTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

		Profiler::Instance().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

		m_Stopped = true;
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#if defined(PROFILE)
#define TRE_PROFILE_BEGIN_SESSION(name, filepath) Profiler::Instance().BeginSession(name, filepath)
#define TRE_PROFILE_END_SESSION() Profiler::Instance().EndSession()
#define TRE_PROFILE_SCOPE(name) ProfileTimer timer##__LINE__(name);
#define TRE_PROFILE_FUNCTION() TRE_PROFILE_SCOPE(TRE_FUNC_SIG)
#else
#define TRE_PROFILE_BEGIN_SESSION(name, filepath)
#define TRE_PROFILE_END_SESSION()
#define TRE_PROFILE_SCOPE(name)
#define TRE_PROFILE_FUNCTION()
#endif

TRE_NS_END