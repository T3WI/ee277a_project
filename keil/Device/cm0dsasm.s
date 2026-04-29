Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=4
Stack_Mem       SPACE   Stack_Size
__initial_sp


Heap_Size       EQU     0x00000400 							
                AREA    HEAP, NOINIT, READWRITE, ALIGN=4
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


; Vector Table Mapped to Address 0 at Reset

						PRESERVE8
                		THUMB

        				AREA	RESET, DATA, READONLY
        				EXPORT 	__Vectors
					
__Vectors		    	DCD		0x00003FFC
        				DCD		Reset_Handler
        				DCD		0  			
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD 	0
        				DCD		0
        				DCD		0
        				DCD 	0
        				DCD		0
        				
        				; External Interrupts
						        				
        				DCD		Timer_Handler
        				DCD		UART_Handler
        				DCD		GPIO7_Handler	
        				DCD		GPIO6_Handler	
        				DCD		GPIO5_Handler	
        				DCD		GPIO4_Handler	
        				DCD		GPIO3_Handler	
        				DCD		GPIO2_Handler	
        				DCD		GPIO1_Handler	
        				DCD		GPIO0_Handler	
        				DCD		0	
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
        				DCD		0
              
                AREA |.text|, CODE, READONLY
;Reset Handler
Reset_Handler   PROC
                GLOBAL Reset_Handler
                ENTRY
				IMPORT  __main
                LDR     R0, =__main               
                BX      R0                        ;Branch to __main
                ENDP

Timer_Handler   PROC
                EXPORT Timer_Handler
				IMPORT Timer_ISR
                PUSH    {R0,R1,R2,LR}
				BL Timer_ISR
                POP     {R0,R1,R2,PC}                    ;return
                ENDP

UART_Handler    PROC
                EXPORT UART_Handler
				IMPORT UART_ISR
                PUSH    {R0,R1,R2,LR}
				BL UART_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO7_Handler   PROC
                EXPORT GPIO7_Handler
				IMPORT GPIO7_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO7_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO6_Handler   PROC
                EXPORT GPIO6_Handler
				IMPORT GPIO6_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO6_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO5_Handler   PROC
                EXPORT GPIO5_Handler
				IMPORT GPIO5_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO5_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO4_Handler   PROC
                EXPORT GPIO4_Handler
				IMPORT GPIO4_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO4_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO3_Handler   PROC
                EXPORT GPIO3_Handler
				IMPORT GPIO3_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO3_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO2_Handler   PROC
                EXPORT GPIO2_Handler
				IMPORT GPIO2_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO2_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO1_Handler   PROC
                EXPORT GPIO1_Handler
				IMPORT GPIO1_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO1_ISR
                POP     {R0,R1,R2,PC}
                ENDP

GPIO0_Handler   PROC
                EXPORT GPIO0_Handler
				IMPORT GPIO0_ISR
                PUSH    {R0,R1,R2,LR}
				BL GPIO0_ISR
                POP     {R0,R1,R2,PC}
                ENDP
				ALIGN 		4					 ; Align to a word boundary

; User Initial Stack & Heap
                IF      :DEF:__MICROLIB
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                ELSE
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

		END                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
   