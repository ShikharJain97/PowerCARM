#include <stdlib.h>
#include "stream.h"
#include "output.h"
#include "utils.h"
#include <immintrin.h>
 #include <omp.h>
static void zero_simd(){
printf("HELLO\n");
printf("^^^^^^^^^^^^^^^^^^^^^ OMP_THREAD_NUM is: %d ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",omp_get_thread_num());
__asm__ __volatile__ (\
	"vpxor %%ymm0, %%ymm0,%%ymm0\n\t"\
	"vpxor %%ymm1, %%ymm1,%%ymm1\n\t"\
	"vpxor %%ymm2, %%ymm2,%%ymm2\n\t"\
	"vpxor %%ymm3, %%ymm3,%%ymm3\n\t"\
	"vpxor %%ymm4, %%ymm4,%%ymm4\n\t"\
	"vpxor %%ymm5, %%ymm5,%%ymm5\n\t"\
	"vpxor %%ymm6, %%ymm6,%%ymm6\n\t"\
	"vpxor %%ymm7, %%ymm7,%%ymm7\n\t"\
	"vpxor %%ymm8, %%ymm8,%%ymm8\n\t"\
	"vpxor %%ymm9, %%ymm9,%%ymm9\n\t"\
	"vpxor %%ymm10, %%ymm10,%%ymm10\n\t"\
	"vpxor %%ymm11, %%ymm11,%%ymm11\n\t"\
	"vpxor %%ymm12, %%ymm12,%%ymm12\n\t"\
	"vpxor %%ymm13, %%ymm13,%%ymm13\n\t"\
	"vpxor %%ymm14, %%ymm14,%%ymm14\n\t"\
	"vpxor %%ymm15, %%ymm15,%%ymm15\n\t"\
	"vpxor %%ymm0, %%ymm0,%%ymm0\n\t"\
	"vpxor %%ymm1, %%ymm1,%%ymm1\n\t"\
	"vpxor %%ymm2, %%ymm2,%%ymm2\n\t"\
	"vpxor %%ymm3, %%ymm3,%%ymm3\n\t"\
	"vpxor %%ymm4, %%ymm4,%%ymm4\n\t"\
	"vpxor %%ymm5, %%ymm5,%%ymm5\n\t"\
	"vpxor %%ymm6, %%ymm6,%%ymm6\n\t"\
	"vpxor %%ymm7, %%ymm7,%%ymm7\n\t"\
	"vpxor %%ymm8, %%ymm8,%%ymm8\n\t"\
	"vpxor %%ymm9, %%ymm9,%%ymm9\n\t"\
	"vpxor %%ymm10, %%ymm10,%%ymm10\n\t"\
	"vpxor %%ymm11, %%ymm11,%%ymm11\n\t"\
	"vpxor %%ymm12, %%ymm12,%%ymm12\n\t"\
	"vpxor %%ymm13, %%ymm13,%%ymm13\n\t"\
	"vpxor %%ymm14, %%ymm14,%%ymm14\n\t"\
	"vpxor %%ymm15, %%ymm15,%%ymm15\n\t"\
	::: "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7", "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15", "memory" );
}

#define BUFFER_SIZE 1024
static __thread const char* command="sudo rdmsr -d 1553";
static __thread unsigned long int msr_val_start=0;
static __thread FILE* temp_start;
static __thread char buffer_start[BUFFER_SIZE];
static __thread char *endptr_start;
static __thread unsigned long int msr_val_end=0;
static __thread FILE* temp_end;
static __thread char buffer_end[BUFFER_SIZE];
static __thread char *endptr_end;
static __thread clock_t valid_start_clik=0;
static __thread clock_t valid_end_clik=0;
void roofline_oi_benchmark(roofline_stream data, roofline_output out, __attribute__ ((unused)) int op_type, long repeat){
zero_simd();


#pragma omp critical
{
temp_start=popen(command, "r");
if (temp_start == NULL) { 
 perror("Err in Popen while reading MSR");
 }
fread(buffer_start, 1, 30, temp_start);
pclose(temp_start);
msr_val_start = strtoul(buffer_start, &endptr_start, 10);
printf(" **** MSR BUFFER BEGIN: %lu *****\n",msr_val_start);power_roofline_output_begin_measure(out,msr_val_start);
}


printf(" \n\nReading Begin Clock Ticks\n\n ");
valid_start_clik=clock();
clik_tick_capture_start(out,valid_start_clik);
roofline_output_begin_measure(out);
__asm__ __volatile__ (\
"loop_STORE_MAD_repeat:\n\t"\
"mov %1, %%r11\n\t"\
"mov %2, %%r12\n\t"\
"buffer_STORE_MAD_increment:\n\t"\
"vmovapd %%ymm0, 0(%%r11)\n\t"\
"vaddpd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vmulpd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vaddpd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vmulpd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vaddpd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmulpd %%ymm6, %%ymm6, %%ymm6\n\t"\
"vaddpd %%ymm7, %%ymm7, %%ymm7\n\t"\
"vmulpd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vmovapd %%ymm9, 32(%%r11)\n\t"\
"vaddpd %%ymm10, %%ymm10, %%ymm10\n\t"\
"vmulpd %%ymm11, %%ymm11, %%ymm11\n\t"\
"vaddpd %%ymm12, %%ymm12, %%ymm12\n\t"\
"vmulpd %%ymm13, %%ymm13, %%ymm13\n\t"\
"vaddpd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmulpd %%ymm15, %%ymm15, %%ymm15\n\t"\
"vaddpd %%ymm0, %%ymm0, %%ymm0\n\t"\
"vmulpd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vmovapd %%ymm2, 64(%%r11)\n\t"\
"vaddpd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vmulpd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vaddpd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmulpd %%ymm6, %%ymm6, %%ymm6\n\t"\
"vaddpd %%ymm7, %%ymm7, %%ymm7\n\t"\
"vmulpd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vaddpd %%ymm9, %%ymm9, %%ymm9\n\t"\
"vmulpd %%ymm10, %%ymm10, %%ymm10\n\t"\
"vmovapd %%ymm11, 96(%%r11)\n\t"\
"vaddpd %%ymm12, %%ymm12, %%ymm12\n\t"\
"vmulpd %%ymm13, %%ymm13, %%ymm13\n\t"\
"vaddpd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmulpd %%ymm15, %%ymm15, %%ymm15\n\t"\
"vaddpd %%ymm0, %%ymm0, %%ymm0\n\t"\
"vmulpd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vaddpd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vmulpd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vmovapd %%ymm4, 128(%%r11)\n\t"\
"vaddpd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmulpd %%ymm6, %%ymm6, %%ymm6\n\t"\
"vaddpd %%ymm7, %%ymm7, %%ymm7\n\t"\
"vmulpd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vaddpd %%ymm9, %%ymm9, %%ymm9\n\t"\
"vmulpd %%ymm10, %%ymm10, %%ymm10\n\t"\
"vaddpd %%ymm11, %%ymm11, %%ymm11\n\t"\
"vmulpd %%ymm12, %%ymm12, %%ymm12\n\t"\
"vmovapd %%ymm13, 160(%%r11)\n\t"\
"vaddpd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmulpd %%ymm15, %%ymm15, %%ymm15\n\t"\
"vaddpd %%ymm0, %%ymm0, %%ymm0\n\t"\
"vmulpd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vaddpd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vmulpd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vaddpd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vmulpd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmovapd %%ymm6, 192(%%r11)\n\t"\
"vaddpd %%ymm7, %%ymm7, %%ymm7\n\t"\
"vmulpd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vaddpd %%ymm9, %%ymm9, %%ymm9\n\t"\
"vmulpd %%ymm10, %%ymm10, %%ymm10\n\t"\
"vaddpd %%ymm11, %%ymm11, %%ymm11\n\t"\
"vmulpd %%ymm12, %%ymm12, %%ymm12\n\t"\
"vaddpd %%ymm13, %%ymm13, %%ymm13\n\t"\
"vmulpd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmovapd %%ymm15, 224(%%r11)\n\t"\
"vaddpd %%ymm0, %%ymm0, %%ymm0\n\t"\
"vmulpd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vaddpd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vmulpd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vaddpd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vmulpd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vaddpd %%ymm6, %%ymm6, %%ymm6\n\t"\
"vmulpd %%ymm7, %%ymm7, %%ymm7\n\t"\
"add $256, %%r11\n\t"\
"sub $256, %%r12\n\t"\
"jnz buffer_STORE_MAD_increment\n\t"\
"sub $1, %0\n\t"\
"jnz loop_STORE_MAD_repeat\n\t"\
:: "r" (repeat), "r" (data->stream), "r" (data->size)\
: "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7", "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15", "%r11", "%r12", "memory" );
roofline_output_end_measure(out, repeat*data->size, repeat*data->size*256/256, repeat*data->size*72/256);
printf(" \n\nReading End Clock Ticks\n\n ");
valid_end_clik=clock();
clik_tick_capture_end(out,valid_end_clik);


#pragma omp critical
{
temp_end=popen(command, "r");
if (temp_end == NULL) { 
 perror("Err in Popen while reading MSR");
 }
fread(buffer_end, 1, 30, temp_end);
pclose(temp_end);
msr_val_end = strtoul(buffer_end, &endptr_end, 10);
printf("**** MSR BUFFER END: %lu ****\n",msr_val_end);

power_roofline_output_end_measure(out,msr_val_end);
}




 /* ****** reintializing string and integer buffers ****** */ 
 
msr_val_start=0;
temp_start=0;
memset(buffer_start,0,sizeof(buffer_start));
memset(buffer_end,0,sizeof(buffer_end));
endptr_start=0;
msr_val_end=0;
temp_end=0;
endptr_end=0;
msr_val_end=0;
valid_start_clik=0;
valid_end_clik=0;


#pragma omp critical
{
temp_start=popen(command, "r");
if (temp_start == NULL) { 
 perror("Err in Popen while reading MSR");
 }
fread(buffer_start, 1, 30, temp_start);
pclose(temp_start);
msr_val_start = strtoul(buffer_start, &endptr_start, 10);
printf(" **** MSR BUFFER BEGIN: %lu *****\n",msr_val_start);power_roofline_output_begin_measure(out,msr_val_start);
}


printf(" \n\nReading Begin Clock Ticks\n\n ");
valid_start_clik=clock();
clik_tick_capture_start(out,valid_start_clik);
roofline_output_begin_measure(out);
__asm__ __volatile__ (\
"loop_STORE_MAD_validation_repeat:\n\t"\
"mov %1, %%r11\n\t"\
"mov %2, %%r12\n\t"\
"buffer_STORE_MAD_validation_increment:\n\t"\
"add $256, %%r11\n\t"\
"sub $256, %%r12\n\t"\
"jnz buffer_STORE_MAD_validation_increment\n\t"\
"sub $1, %0\n\t"\
"jnz loop_STORE_MAD_validation_repeat\n\t"\
:: "r" (1), "r" (data->stream), "r" (data->size)\
: "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7", "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15", "%r11", "%r12", "memory" );
roofline_output_end_measure(out, 0, 0, 0);
printf(" \n\nReading End Clock Ticks\n\n ");
valid_end_clik=clock();
clik_tick_capture_end(out,valid_end_clik);


#pragma omp critical
{
temp_end=popen(command, "r");
if (temp_end == NULL) { 
 perror("Err in Popen while reading MSR");
 }
fread(buffer_end, 1, 30, temp_end);
pclose(temp_end);
msr_val_end = strtoul(buffer_end, &endptr_end, 10);
printf("**** MSR BUFFER END: %lu ****\n",msr_val_end);

power_roofline_output_end_measure(out,msr_val_end);
}




 /* ****** reintializing string and integer buffers ****** */ 
 
msr_val_start=0;
temp_start=0;
memset(buffer_start,0,sizeof(buffer_start));
memset(buffer_end,0,sizeof(buffer_end));
endptr_start=0;
msr_val_end=0;
temp_end=0;
endptr_end=0;
msr_val_end=0;
valid_start_clik=0;
valid_end_clik=0;
}

