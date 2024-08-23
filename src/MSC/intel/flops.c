#include "intel.h"
#include "../../list.h"

/* asm instruction for flops between operand a & b. Result stored into c*/
#define simd_fp(op1, op2)			\
  roofline_flop_ins(op1,"0")			\
  roofline_flop_ins(op2,"1")			\
  roofline_flop_ins(op1,"2")			\
  roofline_flop_ins(op2,"3")			\
  roofline_flop_ins(op1,"4")			\
  roofline_flop_ins(op2,"5")			\
  roofline_flop_ins(op1,"6")			\
  roofline_flop_ins(op2,"7")			\
  roofline_flop_ins(op1,"8")			\
  roofline_flop_ins(op2,"9")			\
  roofline_flop_ins(op1,"10")			\
  roofline_flop_ins(op2,"11")			\
  roofline_flop_ins(op1,"12")			\
  roofline_flop_ins(op2,"13")			\
  roofline_flop_ins(op1,"14")			\
  roofline_flop_ins(op2,"15")			\
  roofline_flop_ins(op1,"0")			\
  roofline_flop_ins(op2,"1")			\
  roofline_flop_ins(op1,"2")			\
  roofline_flop_ins(op2,"3")			\
  roofline_flop_ins(op1,"4")			\
  roofline_flop_ins(op2,"5")			\
  roofline_flop_ins(op1,"6")			\
  roofline_flop_ins(op2,"7")			\
  roofline_flop_ins(op1,"8")			\
  roofline_flop_ins(op2,"9")			\
  roofline_flop_ins(op1,"10")			\
  roofline_flop_ins(op2,"11")			\
  roofline_flop_ins(op1,"12")			\
  roofline_flop_ins(op2,"13")			\
  roofline_flop_ins(op1,"14")			\
  roofline_flop_ins(op2,"15")
  

#define simd_fp_overhead(op1, op2)		\
  roofline_flop_ins(op1,"0")			\
  roofline_flop_ins(op2,"1")			\
  roofline_flop_ins(op1,"2")			\
  roofline_flop_ins(op2,"3")			\
  roofline_flop_ins(op1,"4")			\
  roofline_flop_ins(op2,"5")			\
  roofline_flop_ins(op1,"6")			\
  roofline_flop_ins(op2,"7")			\
  roofline_flop_ins(op1,"8")			\
  roofline_flop_ins(op2,"9")			\
  roofline_flop_ins(op1,"10")			\
  roofline_flop_ins(op2,"11")			\
  roofline_flop_ins(op1,"12")			\
  roofline_flop_ins(op2,"13")			\
  roofline_flop_ins(op1,"14")			\
  roofline_flop_ins(op2,"15")

#define asm_flops(repeat, out, type_str, op1, op2) do{ \
    /*put energy measurement begin code here */   \
    unsigned long int msr_val=read_energy_msr();\
    power_roofline_output_begin_measure(out,msr_val);\
    clik_tick_capture_start(out, clock());\
    roofline_output_begin_measure(out);					\
    __asm__ __volatile__ (						\
      "loop_flops_"type_str"_repeat:\n\t"				\
      simd_fp(op1,op2)							\
      "sub $1, %0\n\t"							\
      "cmp $0, %0\n\t"							\
      "jne loop_flops_"type_str"_repeat\n\t"				\
      :: "r" (repeat)	: SIMD_CLOBBERED_REGS);				\
    roofline_output_end_measure(out, 0, 16*SIMD_FLOPS*repeat, repeat*16); \
    clik_tick_capture_end(out, clock());\
    /*put energy measurement end code here */\
    msr_val=read_energy_msr();\
    power_roofline_output_end_measure(out,msr_val);\
  } while(0)

#define asm_flops_overhead(repeat, out, type_str, op1, op2) do{		\ 
     /*put energy measurement begin code here */ \ 
    unsigned long int msr_val=read_energy_msr();   \
    power_roofline_output_begin_measure(out,msr_val); \
    clik_tick_capture_start(out, clock());\  
    roofline_output_begin_measure(out);					\
    __asm__ __volatile__ (						\
      "loop_flops_"type_str"_overhead_repeat:\n\t"			\
      simd_fp_overhead(op1,op2)						\
      "sub $1, %0\n\t"							\
      "cmp $0, %0\n\t"							\
      "jne loop_flops_"type_str"_overhead_repeat\n\t"			\
      :: "r" (repeat)	: SIMD_CLOBBERED_REGS);				\
    roofline_output_end_measure(out, 0, 0, 0);				\
    clik_tick_capture_end(out, clock());\
    /*put energy measurement end code here */\
    msr_val=read_energy_msr();\
    power_roofline_output_end_measure(out,msr_val);\
  } while(0)


void benchmark_flops(long repeat, roofline_output out, int op_type){
  printf("#### INside MSC/intel/flops.c ; Func:benhcmark_flops #####\n");
  /* Overhead measure */  
  zero_simd();
  switch(op_type){
  case ROOFLINE_ADD:
     printf("^^^^^^^ FLOP TYPE IS: ROOFLINE_ADD ^^^^^^^ \n");
     printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops !!!!!!!!! \n");
    asm_flops(repeat, out, "add", SIMD_ADD, SIMD_ADD);
    printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops_overhead !!!!!!!!! \n");
    asm_flops_overhead(repeat, out, "add", SIMD_ADD, SIMD_ADD);    
    break;
  case ROOFLINE_MUL:
     printf("^^^^^^^ FLOP TYPE IS: ROOFLINE_MUL ^^^^^^^ \n"); 
     printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops !!!!!!!!! \n");
    asm_flops(repeat, out, "mul", SIMD_MUL, SIMD_MUL);
    printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops_overhead !!!!!!!!! \n");
    asm_flops_overhead(repeat, out, "mul", SIMD_MUL, SIMD_MUL);    
    break;
  case ROOFLINE_MAD:
     printf("^^^^^^^ FLOP TYPE IS: ROOFLINE_MAD ^^^^^^^ \n"); 
     printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops !!!!!!!!! \n");
    asm_flops(repeat, out, "mad", SIMD_MUL, SIMD_ADD);
    printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops_overhead !!!!!!!!! \n");
    asm_flops_overhead(repeat, out, "mad", SIMD_MUL, SIMD_ADD);    
    break;
#if defined (__FMA__)    
  case ROOFLINE_FMA:
     printf("^^^^^^^ FLOP TYPE IS: FMA ^^^^^^^ \n"); 
     printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops !!!!!!!!! \n");
     
    
    
    asm_flops(repeat, out, "fma", SIMD_FMA, SIMD_FMA);
    printf(" !!!!!!!!! INside MSC/intel/flops.c; Func: asm_flops_overhead !!!!!!!!! \n");
    asm_flops_overhead(repeat, out, "fma", SIMD_FMA, SIMD_FMA);    
    out->flops*=2;
    printf(" &&&&&&&&&&&&&&&&&&^^^^^^^^^^^^^^^^^^^^^^^********************$$$$$$$$$$$$$$$$$$$$$$$$$  \n\n\n");
    printf(" &&&&&&&&&&&&&&&&&&^^^^^^^^^^^^^^^^^^^^^^^********************$$$$$$$$$$$$$$$$$$$$$$$$$  \n\n\n");
    printf("%ld\n\n\n FMA FLOPS:",out->flops);
    break;
#endif
  default:
    ERR("Not supported type %s\n", roofline_type_str(op_type));
    break;
  }
}

