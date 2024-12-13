#pragma once
#include <thread>
#include <Job/IThreadableJob.h>

class ServerSocketManager;

class UEServerInstanceAcceptConnectionJob : public IThreadableJob
{
public:

	virtual bool InitializeJob() override;
	virtual void RunJob()		 override;
	virtual void TerminateJob()  override;

private:

	void HandleAcceptConnection() const;
	int AcceptConnection() const;

	std::weak_ptr<ServerSocketManager> SocketManager;

};

