#pragma once
#include <thread>
#include <chrono>

#include "cMainStruct.h"

class Thread
{
public:
	Thread() noexcept { }

	/*
	The function must be __cdecl otherwise it could crash or return garbage
	Example of creating a thread
		threadObject.createThread<Arguments>(&function, arguments);
	Example of usage:
		Thread funtionTest;
		funtionTest.createThread<char*>(&function, (char*)"b");
		funtionTest.releaseThread();
	*/

	template<typename ...Args>
	void createThread(void* function, Args... args) noexcept
	{
		currentThread = csgo->createSimpleThread(function, args..., 0U);
	}

	void releaseThread() noexcept
	{
		if (currentThread)
			csgo->releaseThreadHandle(currentThread);
	}

	void* getThreadHandle() noexcept
	{
		return currentThread;
	}

private:
	void* currentThread = nullptr;
};