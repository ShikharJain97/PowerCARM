#include <omp.h>
#include <sampling.h>

roofline_sampling_init("my_CARM_result.roofs", TYPE_LOAD, ROOFLINE_NUMA);

#pragma omp parallel
{
void * sample = roofline_sampling_start(0, hypothetic_flop_count, hypothetic_byte_count);

...
/* The code to evaluate */
...

roofline_sampling_stop (sample, "my_test_code");
}

roofline_sampling_fini();
