/*
Author:   Nelson Luis Moreira da Costa
Email:    nelsonlmdcosta@gmail.com
LinkedIn: http://www.linkedin.com/in/nelsonlmdcosta

Description:
Base Class For The Singleton Design Pattern For Various Systems
Lifetime of the object is when it's requested for the first time till the end of the application lifecycle
*/

#pragma once

#include <memory>

template <typename T>
class ASingleton
{
protected:

	ASingleton() {}
	~ASingleton() {}

public:

	// Disable Copy and Move Constructors and Operators
	ASingleton(ASingleton&& Instance) = delete;
	ASingleton(const ASingleton& Instance) = delete;
	T& operator=(ASingleton<T>&& Instance) = delete;
	T& operator=(const ASingleton<T>& Instance) = delete;

	// Get The Singleton Instance
	// NOTE: Cache this if you use it a lot, this will be a lot slower than keeping it cached
	static T& Instance()
	{
		// Static declaration in here so it's created when called and cleaned up at the end
		//static T singletonInstance;

		return *SingletonInstance.get();
	}

	static std::shared_ptr<T> InstanceAsStrongPointer()
	{
		return SingletonInstance;
	}

	static std::shared_ptr<T> SingletonInstance;
};

template<typename T>
std::shared_ptr<T> ASingleton<T>::SingletonInstance = std::make_shared<T>();
