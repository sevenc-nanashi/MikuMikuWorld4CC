#pragma once
#include <chrono>

namespace MikuMikuWorld
{
	class Stopwatch
	{
	  private:
		using clock_type = std::chrono::steady_clock;
		std::chrono::time_point<clock_type> begin{ clock_type::now() };

	  public:
		void reset();
		double elapsed() const;
		int elapsedMinutes() const;
	};
}