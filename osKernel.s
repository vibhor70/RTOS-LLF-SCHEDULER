				AREA |.text|,CODE,READONLY,ALIGN=2
				THUMB
				PRESERVE8
				EXTERN current_ptr_task
				EXPORT SysTick_Handler
				EXPORT PendSV_Handler 
				IMPORT SCHEDULAR	
					
SysTick_Handler  ;save r0,r1,r2,r3,r12,lr,pc,psr
	CPSID  I;disable any interrupts
	PUSH {R4-R11}
	LDR   R0,=current_ptr_task
	LDR   R1,[R0]
	STR   SP,[R1] ;stack pointer to next top of  thread
	PUSH   {R0,LR}
	BL     SCHEDULAR
	POP    {R0,LR}
	LDR    R1,[R0] ;R1=CURRENT POINTER
	LDR   SP,[R1]    ;SP = CURRENTPT ->STACKPT
	POP   {R4-R11}
	CPSIE  I
	BX     LR ;RESTORED AUTOMATICALLY BY PROCESSOR

PendSV_Handler 
	CPSID  I;disable any interrupts
	PUSH {R4-R11}
	LDR     R0,=current_ptr_task
	LDR     R2,[R0] ;R2 = R2=CURENT PTR
	LDR     SP,[R2]   ;R2 =CURRENT PTR->STACK TOP
	POP     {R4-R11}
	POP     {R0-R3}
	POP      {R12}
	ADD      SP,SP,#4
	POP     {LR}
	ADD      SP,SP,#4
	CPSIE     I
	BX 		 LR
	ALIGN
	END