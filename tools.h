#pragma once

namespace tools
{
	class Stopwatch {
	public:
		Stopwatch(double currentTime = glfwGetTime()) : startTime(currentTime) {}
		double get(double currentTime = glfwGetTime())
		{
			return currentTime - startTime;
		}
		void reset(double currentTime = glfwGetTime())
		{
			startTime = currentTime;
		}
	private:
		double startTime;
	};

	class Counter {
	public:
		Counter(int initValue = 0) : value(initValue) {}
		int get(void)
		{
			return value;
		}
		void set(int newValue = 0)
		{
			value = newValue;
		}
		void inc(void) { value++; }
		void dec(void) { value--; }
	private:
		int value;
	};
}