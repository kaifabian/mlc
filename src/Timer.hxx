#pragma once

#include <chrono>

using namespace std;

template <typename clock, typename resolution_type = float>
class Timer
{
	typedef std::chrono::time_point<clock> time_point;
	typedef std::chrono::duration<resolution_type> duration;

private:
	time_point m_start_time;

public:
	void start()
	{
		this->m_start_time = clock::now();
	}

	resolution_type elapsed() {
		duration d = clock::now() - this->m_start_time;
		std::chrono::nanoseconds s = std::chrono::duration_cast<std::chrono::nanoseconds>(d);
		return ((resolution_type) s.count()) / ((resolution_type) (1000 * 1000 * 1000));
	}
};

typedef Timer<std::chrono::system_clock> SystemTimer;
typedef Timer<std::chrono::high_resolution_clock, double> HighResolutionTimer;
