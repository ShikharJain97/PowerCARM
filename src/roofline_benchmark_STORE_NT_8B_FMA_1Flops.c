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
"loop_STORE_NT_FMA_repeat:\n\t"\
"mov %1, %%r11\n\t"\
"mov %2, %%r12\n\t"\
"buffer_STORE_NT_FMA_increment:\n\t"\
"vmovntpd %%ymm0, 0(%%r11)\n\t"\
"vmovntpd %%ymm1, 32(%%r11)\n\t"\
"vfmadd132pd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vmovntpd %%ymm3, 64(%%r11)\n\t"\
"vmovntpd %%ymm4, 96(%%r11)\n\t"\
"vfmadd132pd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmovntpd %%ymm6, 128(%%r11)\n\t"\
"vmovntpd %%ymm7, 160(%%r11)\n\t"\
"vfmadd132pd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vmovntpd %%ymm9, 192(%%r11)\n\t"\
"vmovntpd %%ymm10, 224(%%r11)\n\t"\
"vfmadd132pd %%ymm11, %%ymm11, %%ymm11\n\t"\
"vmovntpd %%ymm12, 256(%%r11)\n\t"\
"vmovntpd %%ymm13, 288(%%r11)\n\t"\
"vfmadd132pd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmovntpd %%ymm15, 320(%%r11)\n\t"\
"vmovntpd %%ymm0, 352(%%r11)\n\t"\
"vfmadd132pd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vmovntpd %%ymm2, 384(%%r11)\n\t"\
"vmovntpd %%ymm3, 416(%%r11)\n\t"\
"vfmadd132pd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vmovntpd %%ymm5, 448(%%r11)\n\t"\
"vmovntpd %%ymm6, 480(%%r11)\n\t"\
"vfmadd132pd %%ymm7, %%ymm7, %%ymm7\n\t"\
"vmovntpd %%ymm8, 512(%%r11)\n\t"\
"vmovntpd %%ymm9, 544(%%r11)\n\t"\
"vfmadd132pd %%ymm10, %%ymm10, %%ymm10\n\t"\
"vmovntpd %%ymm11, 576(%%r11)\n\t"\
"vmovntpd %%ymm12, 608(%%r11)\n\t"\
"vfmadd132pd %%ymm13, %%ymm13, %%ymm13\n\t"\
"vmovntpd %%ymm14, 640(%%r11)\n\t"\
"vmovntpd %%ymm15, 672(%%r11)\n\t"\
"vfmadd132pd %%ymm0, %%ymm0, %%ymm0\n\t"\
"vmovntpd %%ymm1, 704(%%r11)\n\t"\
"vmovntpd %%ymm2, 736(%%r11)\n\t"\
"vfmadd132pd %%ymm3, %%ymm3, %%ymm3\n\t"\
"vmovntpd %%ymm4, 768(%%r11)\n\t"\
"vmovntpd %%ymm5, 800(%%r11)\n\t"\
"vfmadd132pd %%ymm6, %%ymm6, %%ymm6\n\t"\
"vmovntpd %%ymm7, 832(%%r11)\n\t"\
"vmovntpd %%ymm8, 864(%%r11)\n\t"\
"vfmadd132pd %%ymm9, %%ymm9, %%ymm9\n\t"\
"vmovntpd %%ymm10, 896(%%r11)\n\t"\
"vmovntpd %%ymm11, 928(%%r11)\n\t"\
"vfmadd132pd %%ymm12, %%ymm12, %%ymm12\n\t"\
"vmovntpd %%ymm13, 960(%%r11)\n\t"\
"vmovntpd %%ymm14, 992(%%r11)\n\t"\
"vfmadd132pd %%ymm15, %%ymm15, %%ymm15\n\t"\
"vmovntpd %%ymm0, 1024(%%r11)\n\t"\
"vmovntpd %%ymm1, 1056(%%r11)\n\t"\
"vfmadd132pd %%ymm2, %%ymm2, %%ymm2\n\t"\
"vmovntpd %%ymm3, 1088(%%r11)\n\t"\
"vmovntpd %%ymm4, 1120(%%r11)\n\t"\
"vfmadd132pd %%ymm5, %%ymm5, %%ymm5\n\t"\
"vmovntpd %%ymm6, 1152(%%r11)\n\t"\
"vmovntpd %%ymm7, 1184(%%r11)\n\t"\
"vfmadd132pd %%ymm8, %%ymm8, %%ymm8\n\t"\
"vmovntpd %%ymm9, 1216(%%r11)\n\t"\
"vmovntpd %%ymm10, 1248(%%r11)\n\t"\
"vfmadd132pd %%ymm11, %%ymm11, %%ymm11\n\t"\
"vmovntpd %%ymm12, 1280(%%r11)\n\t"\
"vmovntpd %%ymm13, 1312(%%r11)\n\t"\
"vfmadd132pd %%ymm14, %%ymm14, %%ymm14\n\t"\
"vmovntpd %%ymm15, 1344(%%r11)\n\t"\
"vmovntpd %%ymm0, 1376(%%r11)\n\t"\
"vfmadd132pd %%ymm1, %%ymm1, %%ymm1\n\t"\
"vmovntpd %%ymm2, 1408(%%r11)\n\t"\
"vmovntpd %%ymm3, 1440(%%r11)\n\t"\
"vfmadd132pd %%ymm4, %%ymm4, %%ymm4\n\t"\
"vmovntpd %%ymm5, 1472(%%r11)\n\t"\
"vmovntpd %%ymm6, 1504(%%r11)\n\t"\
"vfmadd132pd %%ymm7, %%ymm7, %%ymm7\n\t"\
"add $1536, %%r11\n\t"\
"sub $1536, %%r12\n\t"\
"jnz buffer_STORE_NT_FMA_increment\n\t"\
"sub $1, %0\n\t"\
"jnz loop_STORE_NT_FMA_repeat\n\t"\
:: "r" (repeat), "r" (data->stream), "r" (data->size)\
: "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7", "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15", "%r11", "%r12", "memory" );
roofline_output_end_measure(out, repeat*data->size, repeat*data->size*192/1536, repeat*data->size*72/1536);
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
"loop_STORE_NT_FMA_validation_repeat:\n\t"\
"mov %1, %%r11\n\t"\
"mov %2, %%r12\n\t"\
"buffer_STORE_NT_FMA_validation_increment:\n\t"\
"add $1536, %%r11\n\t"\
"sub $1536, %%r12\n\t"\
"jnz buffer_STORE_NT_FMA_validation_increment\n\t"\
"sub $1, %0\n\t"\
"jnz loop_STORE_NT_FMA_validation_repeat\n\t"\
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

