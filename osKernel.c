
//Scheduler
//TCB
//stack to hold contents of stack
#define SYSPRI3 (*((volatile uint32_t *)0xE000ED20))//systick prirority reg has same add for all ARM boards
#include "osKernel.h"
#define BUS_FREQUENCY  16000000//we are going to divide 16000 by 1000 and store result in millis prescalar
uint32_t MILLIS;//to get multiply by millis to get quanta
#define PENDCTRL (*((volatile uint32_t *)0xE000ED04))
void SCHEDULAR(void);
#define NUM_OF_THREADS 3
#define STACKSIZE      100
void Pend_trigger(void);
uint32_t SysCount=0;
struct PERIOD{
int32_t CPU;
int32_t DEADLINE;
int32_t PERIOD;
int32_t RELEASE;
int32_t REMAIN;
int32_t task;
};

typedef struct PERIOD periodic;
periodic periodic_tasks[NUM_OF_THREADS];
void addPeriodicTaskInfo(uint32_t CPU,uint32_t DEADLINE,uint32_t PERIOD,uint32_t RELEASE,uint32_t REMAIN,uint32_t task){
	periodic_tasks[task].CPU=CPU;
	periodic_tasks[task].DEADLINE=DEADLINE;
	periodic_tasks[task].PERIOD=PERIOD;
	periodic_tasks[task].RELEASE=RELEASE;
	periodic_tasks[task].REMAIN=REMAIN;
	periodic_tasks[task].task=task;
}

struct tcb{
	
	int32_t *stack_pointer;
	struct tcb *next_pointer;
};

typedef struct tcb tcb_type;
tcb_type tcbs[NUM_OF_THREADS];
tcb_type *current_ptr_task;

int32_t TCB_STACK_GLOBAL[NUM_OF_THREADS][STACKSIZE];

void osKernelStackInit(int i){
	tcbs[i].stack_pointer = &TCB_STACK_GLOBAL[i][STACKSIZE -16]; //stackptr points to top of thread
  TCB_STACK_GLOBAL[i][STACKSIZE -1] = 0x01000000;//run in thumb mode xPSR
	TCB_STACK_GLOBAL[i][STACKSIZE-3] = 0x14141414;  //R14
	TCB_STACK_GLOBAL[i][STACKSIZE-4] = 0x12121212;  //R12
	TCB_STACK_GLOBAL[i][STACKSIZE-5] = 0x03030303;  //R3
	TCB_STACK_GLOBAL[i][STACKSIZE-6] = 0x02020202;  //R2
	TCB_STACK_GLOBAL[i][STACKSIZE-7] = 0x01010101;  //R1
	TCB_STACK_GLOBAL[i][STACKSIZE-8] = 0x00000000;  //R0
	  /*We have to save manually*/	
	TCB_STACK_GLOBAL[i][STACKSIZE-9] = 0x11111111;  //R11
	TCB_STACK_GLOBAL[i][STACKSIZE-10] = 0x10101010; //R10
	TCB_STACK_GLOBAL[i][STACKSIZE-11] = periodic_tasks[i].CPU; //R9 CPU
	TCB_STACK_GLOBAL[i][STACKSIZE-12] = periodic_tasks[i].REMAIN; //R8 remain
	TCB_STACK_GLOBAL[i][STACKSIZE-13] = periodic_tasks[i].RELEASE; //R7 release
	TCB_STACK_GLOBAL[i][STACKSIZE-14] = periodic_tasks[i].PERIOD; //R6 PERIOD
	TCB_STACK_GLOBAL[i][STACKSIZE-15] = periodic_tasks[i].DEADLINE; //R5 DEADLINE
	TCB_STACK_GLOBAL[i][STACKSIZE-16] = 0x04040404; //R4 
}

uint8_t osKernelAddThreads(void(*task0)(void),
																	void(*task1)(void),
																	void(*task2)(void)){
											__disable_irq();//DND atomically	
											/*tcbs[0].next_pointer = &tcbs[1];
											tcbs[1].next_pointer =&tcbs[2];
											tcbs[2].next_pointer =&tcbs[0];*/
osKernelStackInit(0);
TCB_STACK_GLOBAL[0][STACKSIZE -2] =(int32_t)(task0);//PC for task0 	

osKernelStackInit(1);
TCB_STACK_GLOBAL[1][STACKSIZE -2] =(int32_t)(task1);//PC for task0 	

osKernelStackInit(2);
TCB_STACK_GLOBAL[2][STACKSIZE -2] =(int32_t)(task2);//PC for task0 

current_ptr_task = &tcbs[0];
__enable_irq();																		
		return 1;																
}
																	
void osKernelInit(void){
	__disable_irq();
	MILLIS = BUS_FREQUENCY/1000;
	NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest possible priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */
}

void isKernelLaunch(uint32_t quanta){
SysTick->CTRL =0;
SysTick->VAL =0;
//SYSPRI3 =(SYSPRI3&0x00FFFFFF) | 0xE0000000;//priority	
SysTick->LOAD =(quanta * MILLIS)-1;//ALL COUNTING BEGINS FROM 0 SO -1
	SysCount = SysCount+((quanta * MILLIS)-1);
	SysTick->CTRL = 0x00000007;//trigger interrupt
	Pend_trigger();
}

void Pend_trigger(void){
	//PENDCTRL  =  0x10000000;
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void SCHEDULAR(void){
	uint32_t laxitvity=0;
	uint32_t min_lax=0;
	uint32_t recent;
	for(int i=0;i<3;i++){
			if((SysCount%TCB_STACK_GLOBAL[i][STACKSIZE-14])-TCB_STACK_GLOBAL[i][STACKSIZE-13] ==0){
				TCB_STACK_GLOBAL[i][STACKSIZE-15] = SysCount + TCB_STACK_GLOBAL[i][STACKSIZE-14];
				TCB_STACK_GLOBAL[i][STACKSIZE-12] = TCB_STACK_GLOBAL[i][STACKSIZE-11];
				
			}
			if(TCB_STACK_GLOBAL[i][STACKSIZE-12] >0){
				laxitvity = TCB_STACK_GLOBAL[i][STACKSIZE-15]-(((SysCount%TCB_STACK_GLOBAL[i][STACKSIZE-14])-TCB_STACK_GLOBAL[i][STACKSIZE-13])+TCB_STACK_GLOBAL[i][STACKSIZE-12]);
				if(min_lax > laxitvity){
					recent = i;
					min_lax =laxitvity;
				}
			}
	}
	if(TCB_STACK_GLOBAL[recent][STACKSIZE-12] !=0){
		TCB_STACK_GLOBAL[recent][STACKSIZE-12]--;
	}
	if(current_ptr_task != &tcbs[recent]){
		current_ptr_task = &tcbs[recent];
		//current_ptr_task = (tcbs +recent*sizeof(tcbs[recent]));
	}
	else{
		 tcbs[recent].next_pointer = current_ptr_task;
	}
	
}