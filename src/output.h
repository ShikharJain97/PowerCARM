#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <inttypes.h>
#include <hwloc.h>
#include <time.h>

typedef struct roofline_output_s{
  /* All sample type specific data */
  uint64_t ts_high_start, ts_low_start;    /* Timestamp in cycles where the roofline started */
  uint64_t ts_high_end, ts_low_end;        /* Timestamp in cycles where the roofline ended */
  uint64_t cycles;                         /* Cycles elapsed: ts_end - ts_start  */    
  uint64_t overhead;                       /* Cycles of spent in overhead */  
  uint64_t instructions;                   /* The number of instructions */
  uint64_t bytes;                          /* The amount of bytes transfered */
  uint64_t flops;                          /* The amount of flops computed */
  unsigned n;                              /* The number of accumulated samples */
  hwloc_obj_t    mem_location;             /* Location where data is allocated */
  hwloc_obj_t    thr_location;             /* Location where thread collecting and accumulating data is pinned */

  uint64_t energy_msr_start; /* Read the value of pkg_energy_msr (decimal adress 1553) at start of the benchmark*/
  uint64_t energy_msr_end;/* Read the value of pkg_energy_msr (decimal adress 1553) at end of the benchmark*/

  unsigned long long int start_clktik; /* To get the clockticks at the time of start of micro benchmark*/
  unsigned long long int end_clktik; /* To get the clockticks at the time of start of micro benchmark*/

   
} * roofline_output;

//////////////////// Counting for one thread
roofline_output new_roofline_output(hwloc_obj_t mem_location);
void            delete_roofline_output(roofline_output);
void            roofline_output_clear(roofline_output);
void            roofline_output_accumulate(roofline_output dst, const roofline_output src);
float           roofline_output_throughput(const roofline_output);
int             roofline_compare_throughput(const void * x, const void * y);
int             roofline_compare_cycles(const void * x, const void * y);
void            roofline_output_begin_measure(roofline_output);
void            roofline_output_end_measure(roofline_output o, const uint64_t bytes, const uint64_t flops, const uint64_t ins);
void            roofline_output_print_header(FILE * output);
void            roofline_output_print(FILE * output, const roofline_output sample_out, const int type); 
void            power_roofline_output_begin_measure(roofline_output o, unsigned long int msr_val);
void            power_roofline_output_end_measure(roofline_output o, unsigned long int msr_val);
unsigned long   read_energy_msr();
void            clik_tick_capture_start(roofline_output o, clock_t valid_start_clik);
void            clik_tick_capture_end(roofline_output o, clock_t valid_end_clik);

//////////////////// Accumulating results by topology group with automatic location detection.
roofline_output roofline_output_init();
void roofline_output_aggregate_result(roofline_output results, const roofline_output src);
void roofline_output_fini(const roofline_output output);
void roofline_print_outputs(FILE * output, const roofline_output outputs, const int op_type);


#endif /* OUTPUT_H */

