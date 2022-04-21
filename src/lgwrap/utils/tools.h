#pragma once

#include <iostream>
#include <GLFW/glfw3.h>

namespace lgw {
	// basic stopwatch
	class Stopwatch {
	public:
		inline Stopwatch(double currentTime = glfwGetTime()) : startTime(currentTime) {}
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
		inline Counter(int initValue = 0) : value(initValue) {}
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

	// toggle
	class Toggle {
	public:
		bool val;
		bool con;
		inline Toggle(bool initValue = false, bool initCondition = true) : val(initValue), con(initCondition) {}
		inline void toggle(void) { val = !val; }
	};
}