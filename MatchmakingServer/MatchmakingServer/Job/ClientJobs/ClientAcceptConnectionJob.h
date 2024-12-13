#pragma once
#include <thread>
#include <Job/IThreadableJob.h>

class ServerSocketManager;

class ClientAcceptConnectionJob : public IThreadableJob
{
public:

	virtual bool InitializeJob() override;
	virtual void RunJob()		 override;
	virtual void TerminateJob()  override;

private:

	void HandleAcceptConnection();
	int AcceptConnection();

	std::weak_ptr<ServerSocketManager> SocketManager;
};

