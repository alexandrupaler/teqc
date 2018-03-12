#include "utils/profiling.h"

double profiling::cpuTime = 0;

double profiling::util_get_cpu_time()
{
    return (double)clock() / CLOCKS_PER_SEC;
}

void profiling::util_get_cpu_time_begin()
{
    profiling::cpuTime = profiling::util_get_cpu_time();
}

double profiling::util_get_cpu_time_end()
{
    return profiling::util_get_cpu_time() - cpuTime;
}
