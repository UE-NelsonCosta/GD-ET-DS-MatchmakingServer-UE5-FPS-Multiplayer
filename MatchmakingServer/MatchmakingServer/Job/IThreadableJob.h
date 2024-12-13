#pragma once

#include <thread>

// Interface For Job Objects These Should All Be Handled By The Main Thread
class IThreadableJob
{
public:
	virtual ~IThreadableJob() = default;

	virtual bool InitializeJob()	= 0;
	virtual void RunJob()			= 0;
	virtual void TerminateJob()		= 0;

	virtual bool IsJobComplete() { return JobComplete; }
	virtual bool WasJobSuccessful() { return WasSuccessful; }

public:

	std::thread Worker;

protected:

	bool JobComplete = false;
	bool WasSuccessful = true;

};

