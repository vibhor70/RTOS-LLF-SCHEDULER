#ifndef _OS_KERNEL_H
#define _OS_KERNEL_H
#include "stm32f4xx.h"                  // Device header

#include<stdint.h>//for uint32_t uint8_t
void isKernelLaunch(uint32_t quanta);

void osKernelInit(void);

uint8_t osKernelAddThreads(void(*task0)(void),
																	void(*task1)(void),
																	void(*task2)(void));
void addPeriodicTaskInfo(uint32_t CPU,uint32_t DEADLINE,uint32_t PERIOD,uint32_t RELEASE,uint32_t REMAIN,uint32_t task);
#endif
																	
//main will be only able to acess this file