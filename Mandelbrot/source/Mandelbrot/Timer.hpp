#pragma once
#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public:
	inline void start()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_bRunning = true;
	}

	inline void stop()
	{
		m_EndTime = std::chrono::high_resolution_clock::now();
		m_bRunning = false;
	}

	inline double elapsedMilliseconds()
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

		if (m_bRunning)
		{
			endTime = std::chrono::high_resolution_clock::now();
		}
		else
		{
			endTime = m_EndTime;
		}

		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count());
	}

	inline double elapsedSeconds()
	{
		return elapsedMilliseconds() / 1000.0;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;
	bool m_bRunning = false;
};

#endif