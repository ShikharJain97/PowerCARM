#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <malloc.h>
//cpu_set_t  mask;
//CPU_ZERO(&mask);
//CPU_SET(0, &mask);
//CPU_SET(2, &mask);
// last level cache size in MB
# define uint32_t unsigned int
# define uint64_t unsigned long int
unsigned int llc_size=1024;
int cnt=1;
/*inline static unsigned long long  get_ticks() {
		unsigned int  low, high;
		__asm__ __volatile__ (
			"xor %%eax, %%eax;"
			"cpuid;"
			"rdtsc;"
		: "=a"(low), "=d"(high)
		:
		: "%rbx", "%rcx"
		);
		return ((unsigned long long(high) << 32) | unsigned long long(low));
	}

inline static unsigned long long release_ticks() {
		unsigned int low, high;
		__asm__ __volatile__ (
			"rdtscp;"
		: "=a"(low), "=d"(high)
		:
		: "%rcx"
		);
		return ((unsigned long long(high) << 32) | unsigned long long(low));
	}
*/
int main()
{
cpu_set_t  mask;
CPU_ZERO(&mask);
CPU_SET(0, &mask);	
int result = sched_setaffinity(0, sizeof(mask), &mask);	
long long int array_size=llc_size*(1024)*(1024)/sizeof(double),t=0,p,q;
//long long int rand_access[20];
double* A=(double*)calloc(3*array_size,sizeof(double));
int temp=0.0;

//get_ticks();
system("sudo wrmsr -a 420 15");
system("sudo rdmsr -a -d 420");
while(cnt<1000000)
{

p=rand()%(array_size/1024);	
t=(rand()%array_size)+array_size;
q=((p*t+rand())%array_size)+2*array_size;
//intf("%lld %lld %lld",p,t,q);
//A[p]=9.0;
//A[array_size+t]=99.8;
//for(int z=0;z<1000000;z++)
temp=A[t]+A[p]+A[q];
cnt++;
//printf("*** %d ***\n",cnt);
}
system("sudo wrmsr -a 420 0");
//release_ticks();
}
