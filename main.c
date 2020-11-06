#include "osKernel.h"
#define quanta 1//checking after every 1 unit of time
int __svc(0x00) svc_service_init();
int __svc(0x01) svc_service_kernelLaunch();
uint32_t count0=0,count1=0,count2=0;
void SVC_Handler_C(unsigned int * svc_args);
void task0(void){
	while(1){
		
	}
}

void task1(void){
	while(1){
		
	}
}

void task2(void){
	while(1){
		
	}
}
int main(void){
	/*
	osKernelInit();
	addPeriodicTaskInfo(2,4,6,0,0,0);
	addPeriodicTaskInfo(4,6,8,0,0,1);
	addPeriodicTaskInfo(6,8,10,0,0,2);
	osKernelAddThreads(&task0,&task1,&task2);
	isKernelLaunch(quanta);
	*/
	svc_service_init();
	svc_service_kernelLaunch();
	
}

__asm void SVC_Handler(void)
{
  
  TST lr, #4
  ITE EQ
  MRSEQ r0, MSP
  MRSNE r0, PSP
	IMPORT SVC_Handler_C
  B SVC_Handler_C
}

void SVC_Handler_C(unsigned int * svc_args){
	unsigned int svc_number;
	svc_number = ((char*)svc_args[6])[-2];
	switch(svc_number){
		case 0:
			osKernelInit();
			addPeriodicTaskInfo(2,4,6,0,0,0);
			addPeriodicTaskInfo(4,6,8,0,0,1);
			addPeriodicTaskInfo(6,8,10,0,0,2);
			osKernelAddThreads(&task0,&task1,&task2);
		break;
		case 1:
			isKernelLaunch(quanta);
			break;
		default:
			break;
		
	}
}