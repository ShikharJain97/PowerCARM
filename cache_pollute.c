#include<stdio.h>
#include<stdlib.h>
// last level cache size in MB
unsigned int llc_size=33;
int cnt=1;
int main()
{
long long int array_size=llc_size*(1024)*(1024)/sizeof(double);
long long int rand_access[20];
long long int y1,y2;
for(int x=0;x<20;)
{
y1=rand();
y2=rand();
if(abs(y1-y2)<array_size)
	continue;
else
{
rand_access[x]=y1;
rand_access[x+1]=y2;
x=x+2;
}
}

for (int x=0;x<20;x++)
printf(" %lld ",rand_access[x]);
printf("\n");
double* A=(double*)calloc(2*array_size,sizeof(double));
long long int i=0,j=array_size+10;
double t=0.0;

while(cnt<10)
{
for(int x=0;x<20;x=x+2)
{	
t=A[x];
t=A[x+1];
}
cnt++;
}

}

