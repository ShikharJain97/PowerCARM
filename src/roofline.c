#include <math.h>
#include "roofline.h"
#include "topology.h"
#include "stream.h"
#include "output.h"
#include "utils.h"
#include "stats.h"
#include "MSC/MSC.h"

hwloc_topology_t topology = NULL;            /* Current machine topology  */
size_t           alignement = 0;             /* Level 1 cache line size */
float            cpu_freq = 0;               /* In Hz */
unsigned         n_threads = 1;              /* The number of threads for benchmark */
unsigned int     roofline_types;             /* What rooflines do we want in byte array */

static LARM_policy policy;                   /* The data allocation policy when target memory is larger than one node */ 
static hwloc_obj_type_t leaf_type;

void roofline_set_root(const hwloc_obj_t obj){
  n_threads = 1;
  int tid = 0;

#if defined(_OPENMP)
  n_threads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, obj->cpuset, leaf_type);
  omp_set_num_threads(n_threads);
#pragma omp parallel
  {    
#pragma omp critical
    {
      tid = omp_get_thread_num();
#endif
      hwloc_obj_t cpu = hwloc_get_obj_inside_cpuset_by_type(topology, obj->cpuset, leaf_type, tid);
      roofline_hwloc_cpubind(cpu);
#ifdef DEBUG1
      fprintf(stderr,"Thread %d bound to %s %d\n", tid, hwloc_obj_type_string(cpu->type), cpu->logical_index);
#endif    
#if defined(_OPENMP)
    }
  }
#endif
}

#if defined(_OPENMP)
  int roofline_lib_init(hwloc_topology_t topo,
			const char * threads_location,
			int with_hyperthreading,
			LARM_policy p)
#else
  int roofline_lib_init(hwloc_topology_t topo,
			const char * threads_location,
			__attribute__ ((unused)) int with_hyperthreading,
			LARM_policy p)
#endif
{
  /* Check hwloc version */
  roofline_hwloc_check_version();

  /* Initialize topology */
  if(topo != NULL) hwloc_topology_dup(&topology, topo);
  else{
    if(hwloc_topology_init(&topology) ==-1){
      fprintf(stderr, "hwloc_topology_init failed.\n");
      return -1;
    }
    hwloc_topology_set_icache_types_filter(topology, HWLOC_TYPE_FILTER_KEEP_ALL);
    if(hwloc_topology_load(topology) ==-1){
      fprintf(stderr, "hwloc_topology_load failed.\n");
      return -1;
    }
  }

  /* Set the type of leaf (Core or PU) */
  leaf_type = with_hyperthreading ? HWLOC_OBJ_PU:HWLOC_OBJ_CORE;

  /* Set root location and bind threads */
  hwloc_obj_t location;
  /* Get first node and number of threads */
  if(threads_location == NULL){
    location = roofline_hwloc_memory_group(0);
  }
  else{
    location = roofline_hwloc_parse_obj(threads_location);    
    while(location && location->arity == 0){ location = location->parent; }
    
    if(location == NULL){
      roofline_mkstr(root_str, 32);
      roofline_hwloc_obj_snprintf(location, root_str, sizeof(root_str));
      fprintf(stderr, "hwloc obj %s has no children\n", root_str);
      goto lib_err_with_topology;
    }
  }
  roofline_set_root(location);
  
  /* get first cache linesize */
  hwloc_obj_t L1 = hwloc_get_obj_by_type(topology, HWLOC_OBJ_L1CACHE, 0);
  if(L1==NULL) ERR_EXIT("No cache found.\n");
  alignement = L1->attr->cache.linesize;

  /* Set allocation policy */
  policy = p;
  
  /* Check if cpu frequency has been defined */
#ifndef CPU_FREQ
  char* cpu_freq_str = getenv("CPU_FREQ");
  if(cpu_freq_str == NULL)
    ERR_EXIT("Please define the machine cpu frequency (in Hertz) with build option CPU_FREQ or the environement variable of the same name");
  cpu_freq = atof(cpu_freq_str);
#else 
  cpu_freq = CPU_FREQ;
#endif 

  return 0;

lib_err_with_topology:
  hwloc_topology_destroy(topology);
  return -1;
}

inline void roofline_lib_finalize(void)
{
  hwloc_topology_destroy(topology);
}

void roofline_fpeak(FILE * output, int op_type)
{
  roofline_output out = roofline_output_init();
  long repeat = roofline_autoset_repeat(NULL, op_type, NULL);

#ifdef _OPENMP
#pragma omp parallel
  {
#endif
    
    int i;
    roofline_output local_out  = new_roofline_output(NULL);    
    for(i=0; i<ROOFLINE_N_SAMPLES; i++){
      printf("#### inside roofline.c ; Func: roofline_fpeak  ######\n");
      roofline_output_clear(local_out);      
      benchmark_fpeak(op_type, local_out, repeat);
      
#ifdef _OPENMP
#pragma omp barrier
#pragma omp critical
#endif      
      roofline_output_aggregate_result(out, local_out);
#ifdef _OPENMP
#pragma omp barrier
#pragma omp single	
#endif
      roofline_print_outputs(output, out, op_type);
    }
    delete_roofline_output(local_out);
#ifdef _OPENMP    
  }
#endif
  roofline_output_fini(out);
}

static void roofline_memory(FILE * output, const hwloc_obj_t memory, const int op_type, void * benchmark)
{
  size_t * sizes, low_size, up_size;
  roofline_output out = roofline_output_init();
  long repeat = 100;
  void (*  benchmark_function)(roofline_stream, roofline_output, int, long) = benchmark;
  static int stop = 0;
    
  /* Generate samples size */
  int n_sizes = ROOFLINE_N_SAMPLES;
  if(roofline_hwloc_get_memory_bounds(memory, &low_size, &up_size, op_type) == -1){ return; }
  sizes = roofline_linear_sizes(op_type, low_size, up_size, &n_sizes);
  if(sizes==NULL){roofline_debug2("Computing array of input sizes from %lu to %lu failed\n", low_size, up_size); return;}
#ifdef DEBUG2
  roofline_debug2("linear array of sizes from %lu to %lu of %d sizes\n", low_size, up_size, n_sizes);
  do{int i;
    for(i=0;i<n_sizes;i++){roofline_debug2("%lu ", sizes[i]);}
    roofline_debug2("\n");
  } while(0);
#endif
  
#ifdef _OPENMP
#pragma omp parallel
  {
#pragma omp barrier
#endif
    if(stop) goto end_parallel_mem_bench;

    /*Initialize IO */    
    roofline_stream src        = new_roofline_stream(up_size, op_type);
    hwloc_obj_t mem_loc        = roofline_hwloc_set_area_membind(memory, src->stream, src->alloc_size, policy);
    roofline_output local_out  = new_roofline_output(memory->depth<roofline_hwloc_memory_group_depth()?mem_loc:memory);
    
    /* measure for several sizes inside bounds */
    int i; for(i=0;i<n_sizes;i++){      
      /* Set buffer size */
      if(sizes[i] < low_size || (i>0 && sizes[i] == sizes[i-1])){ goto skip_size; }    
      src->size = sizes[i];
      if(op_type == ROOFLINE_LATENCY_LOAD){roofline_set_latency_stream(src, src->size);}

#ifdef _OPENMP
#pragma omp single
      {
#endif
#ifdef DEBUG2
	
	if(sizes[i]>GB)
	  roofline_debug2("size = %luGB per thread, %luGB total per %s of size %luGB\n",
			  sizes[i]/GB,
			  sizes[i]*n_threads/GB,
			  hwloc_obj_type_string(memory->type),
			  roofline_hwloc_memory_size(memory)/GB);
	else if(sizes[i]>MB)
	  roofline_debug2("size = %luMB per thread, %luMB total per %s of size %luMB\n",
			  sizes[i]/MB,
			  sizes[i]*n_threads/MB,
			  hwloc_obj_type_string(memory->type),
			  roofline_hwloc_memory_size(memory)/MB);	
	else if(sizes[i]>KB)
	  roofline_debug2("size = %luKB per thread, %luKB total per %s of size %luKB\n",
			  sizes[i]/KB,
			  sizes[i]*n_threads/KB,
			  hwloc_obj_type_string(memory->type),
			  roofline_hwloc_memory_size(memory)/KB);
	else
	  roofline_debug2("size = %luB per thread, %luB total per %s of size %luB\n",
			  sizes[i],
			  sizes[i]*n_threads,
			  hwloc_obj_type_string(memory->type),
			  roofline_hwloc_memory_size(memory));
#endif
#ifdef _OPENMP
	
      }
#endif
	
      /* Set the length of the benchmark to have a small variance */
     // printf("22222222222222222222222222222222222222222222222222222222222222222222222222222222\n");
      if(repeat>1){repeat = roofline_autoset_repeat(src, op_type, benchmark);}

      /* Clear output */
      roofline_output_clear(local_out);    
      
      //printf("3333333333333333333333333333333333333333333333333333333333333333333333333333333333333\n");

      /* Benchmark */
      if(op_type == ROOFLINE_LOAD    || op_type == ROOFLINE_LOAD_NT ||
	 op_type == ROOFLINE_STORE   || op_type == ROOFLINE_STORE_NT||
	 op_type == ROOFLINE_2LD1ST)
      {
      
	benchmark_stream(src, local_out, op_type, repeat);
	//printf("4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444\n");
      } else if(op_type == ROOFLINE_LATENCY_LOAD){
	roofline_latency_stream_load(src, local_out, 0, repeat);
	//printf("55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555\n");	
      } else if(benchmark != NULL){
      //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SAYAN DADA @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	benchmark_function(src, local_out, op_type, repeat);
      }

#ifdef _OPENMP
#pragma omp critical
      {
#endif
#ifdef DEBUG2
      roofline_output_print(output, local_out, op_type);      
#endif      
      roofline_output_aggregate_result(out, local_out);
      
#ifdef _OPENMP
      }
#pragma omp barrier
#pragma omp single
#endif
      roofline_print_outputs(output, out, op_type);
      
    skip_size:;
    }

  end_parallel_mem_bench:
    delete_roofline_stream(src);
    delete_roofline_output(local_out);    
#ifdef _OPENMP      
  }
#endif
  /* Cleanup */
  roofline_output_fini(out);
}


void roofline_bandwidth(FILE * output, const hwloc_obj_t mem, const int type){
  char mem_str[128]; memset(mem_str, 0, sizeof(mem_str));
  roofline_hwloc_obj_snprintf(mem,mem_str,sizeof(mem_str));
  
  int restrict_type = roofline_filter_types(mem, type);
  if(restrict_type & ROOFLINE_LOAD){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_LOAD));
    roofline_memory(output,mem,ROOFLINE_LOAD, NULL);
  }
  if(restrict_type & ROOFLINE_LOAD_NT){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_LOAD_NT));
    roofline_memory(output,mem,ROOFLINE_LOAD_NT, NULL);
  }
  if(restrict_type & ROOFLINE_STORE){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_STORE));
    roofline_memory(output,mem,ROOFLINE_STORE, NULL);
  }
  if(restrict_type & ROOFLINE_STORE_NT){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_STORE_NT));
    roofline_memory(output,mem,ROOFLINE_STORE_NT, NULL);
  }
  if(restrict_type & ROOFLINE_2LD1ST){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_2LD1ST));
    roofline_memory(output,mem,ROOFLINE_2LD1ST, NULL);
  }
  if(restrict_type & ROOFLINE_LATENCY_LOAD){
    printf("benchmark %s memory level for %-4s operation\n", mem_str, roofline_type_str(ROOFLINE_LATENCY_LOAD));
    roofline_memory(output,mem,ROOFLINE_LATENCY_LOAD, NULL);
  }  
}

void roofline_flops(FILE * output, const int type){
  hwloc_obj_t core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, 0);
  int restrict_type = roofline_filter_types(core, type);   
  if(restrict_type & ROOFLINE_ADD){
    printf("benchmark compute unit for %-8s operation\n", roofline_type_str(ROOFLINE_ADD));
    roofline_fpeak(output,ROOFLINE_ADD);
  }
  if(restrict_type & ROOFLINE_MUL){
    printf("benchmark compute unit for %-8s operation\n", roofline_type_str(ROOFLINE_MUL));
    roofline_fpeak(output,ROOFLINE_MUL);
  }
  if(restrict_type & ROOFLINE_MAD){
    printf("benchmark compute unit for %-8s operation\n", roofline_type_str(ROOFLINE_MAD));
    roofline_fpeak(output,ROOFLINE_MAD);
  }
  if(restrict_type & ROOFLINE_FMA){
    printf("benchmark compute unit for %-8s operation\n", roofline_type_str(ROOFLINE_FMA));
    roofline_fpeak(output,ROOFLINE_FMA);
  }
}

void roofline_oi(FILE * output, const hwloc_obj_t mem, const int type, const unsigned flops, const unsigned bytes){
  void * bench;
  int i, j, mem_type, flop_type, t;
  hwloc_obj_t core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, 0);
  const int mem_types[6] = {ROOFLINE_LOAD, ROOFLINE_LOAD_NT, ROOFLINE_STORE, ROOFLINE_STORE_NT, ROOFLINE_2LD1ST};
  const int flop_types[4] = {ROOFLINE_ADD, ROOFLINE_MUL, ROOFLINE_MAD, ROOFLINE_FMA};
  char mem_str[128]; memset(mem_str, 0, sizeof(mem_str));
  roofline_hwloc_obj_snprintf(mem,mem_str,sizeof(mem_str));

  flop_type = roofline_filter_types(core, type);
  mem_type = roofline_filter_types(mem, type);
     
  for(i=0;i<6;i++){
    if(mem_types[i] & mem_type){
      for(j=3;j>=0;j--){
	if(flop_types[j] & flop_type){
	  printf("validation of %-4s(%uFlops) and %-8s(%uBytes) operations on %s memory level\n",
		 roofline_type_str(flop_types[j]), flops, roofline_type_str(mem_types[i]), bytes, mem_str);
	  t = flop_types[j]|mem_types[i];
	  bench = benchmark_validation(t, flops, bytes);
	  if(bench == NULL){continue;}
	  roofline_memory(output, mem, t, bench);
	  break;
	}
      }
    }
  }
}

