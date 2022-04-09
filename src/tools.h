#pragma once

#include <iostream> // for debug

namespace tools
{
	// basic stopwatch
	class Stopwatch {
	public:
		Stopwatch(double currentTime = glfwGetTime()) : startTime(currentTime) {}
		inline double get(double currentTime = glfwGetTime())
		{
			return currentTime - startTime;
		}
		inline void reset(double currentTime = glfwGetTime())
		{
			startTime = currentTime;
		}
	private:
		double startTime;
	};

	// basic counter
	class Counter {
	public:
		Counter(int initValue = 0) : value(initValue) {}
		inline int get(void)
		{
			return value;
		}
		inline void set(int newValue = 0)
		{
			value = newValue;
		}
		inline void inc(void) { value++; }
		inline void dec(void) { value--; }
	private:
		int value;
	};
}