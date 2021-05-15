#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/String/String.hpp"
#include "Core/DataStructure/Queue/Queue.hpp"
#include "Core/FileSystem/File/File.hpp"
#include <functional>
#include <thread>
#include <mutex>

TRE_NS_START

struct AsyncFileRequest
{
	enum RequestType
	{
		READ_STRING, WRITE, PUT, GET_LINE, READ_BYTES, OPEN_FILE
	};

	typedef ::std::function<void(const AsyncFileRequest&)> CallBackFunction;

	AsyncFileRequest(File* h, const RequestType t, CallBackFunction& c, char* b, usize sz = 1) : 
		fHandler(h), callback(c), rType(t)	
	{
		args.buffer = b;
		args.size = sz;
	}

	AsyncFileRequest(File* h, const RequestType t, CallBackFunction& c, void* p, usize sz, usize count = 1) : 
		fHandler(h), callback(c), rType(t)	
	{
		args.ptr = p;
		args.sz = sz;
		args.count = count;
	}

	AsyncFileRequest(File* h, const RequestType t, CallBackFunction& c, String* line) : 
		fHandler(h), callback(c), rType(t)	
	{
		args.line = line;
	}

	AsyncFileRequest(File* h, const RequestType t, CallBackFunction& c, String* path, File::Options options) : 
		fHandler(h), callback(c), rType(t)	
	{
		args.path = path;
		args.options = options;
	}

	File* fHandler;
    CallBackFunction& callback;
	const RequestType rType;

    union Args
    {
        struct{             // ReadString, Write, Put
            char* buffer;
            usize size;
        };

        struct{             // ReadBytes
            void* ptr;
            usize sz;
            usize count;
        };

        struct{             // GetLine
            String* line;    
        };

		struct{				// LoadFile
			String* path;
			File::Options options;
		};
    };

    Args args;
};

struct AsynFileManager
{
	template<AsyncFileRequest::RequestType REQUEST_TYPE, typename... Args>
	AsyncFileRequest& AddRequest(File* f, AsyncFileRequest::CallBackFunction& callback, Args&&... args)
	{
		mtx.lock();
		auto& res = m_Requests.Emplace(f, REQUEST_TYPE, callback, ::std::forward<Args>(args)...);
		mtx.unlock();
		return res;
	}

	void HandleRequest(AsyncFileRequest::RequestType request_type, AsyncFileRequest* r);

	void BootUp()
	{
		m_Thread = ::std::thread(&AsynFileManager::ProcessRequests, this);
		//this->ProcessRequests();
		m_Thread.detach();
	}

	void ProcessRequests();

	void Shutdown()
	{
		m_Thread.join();
	}

	Queue<AsyncFileRequest> m_Requests;
	::std::thread m_Thread;
	::std::mutex mtx;
};

TRE_NS_END