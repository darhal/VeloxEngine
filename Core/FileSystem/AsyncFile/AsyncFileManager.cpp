#include "AsyncFileManager.hpp"

TRE_NS_START

void AsynFileManager::ProcessRequests()
{
	while (true){
		mtx.lock();
		AsyncFileRequest* req = m_Requests.Front();
		mtx.unlock();

		if (req != NULL) {
			this->HandleRequest(req->rType, req);
			mtx.lock();
			m_Requests.Pop();
			mtx.unlock();
		}

		//std::this_thread::sleep_for();
	}
}

void AsynFileManager::HandleRequest(AsyncFileRequest::RequestType request_type, AsyncFileRequest* r)
{
	switch (request_type){
		case AsyncFileRequest::READ_STRING :
			r->fHandler->ReadString(r->args.buffer, r->args.size);
			break;

		case AsyncFileRequest::WRITE :
			r->fHandler->Write(r->args.buffer);
			break;

		case AsyncFileRequest::PUT :
			r->fHandler->Put(r->args.buffer);
			break;

		case AsyncFileRequest::READ_BYTES :
			r->fHandler->ReadBytes(r->args.ptr, r->args.sz, r->args.count);
			break;

		case AsyncFileRequest::GET_LINE :
			r->fHandler->GetLine(r->args.line);
			break;

		case AsyncFileRequest::OPEN_FILE :
			r->fHandler->LoadFile(*r->args.path, r->args.options);
			break;

		default:
			return;
	}
	r->callback(*r);
}

TRE_NS_END