#ifndef PROFILING_H_
#define PROFILING_H_

#include <sys/time.h>
#include <time.h>

class profiling
{
private:
	static double cpuTime;

public:
	static double util_get_cpu_time();

	static void util_get_cpu_time_begin();

	static double util_get_cpu_time_end();
};

#endif /* PROFILING_H_ */
