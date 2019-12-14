; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; Ver 1 19/03/2018
; Ver 2 26/08/2018
; Este programa deve esperar o usuário pressionar uma chave.
; Caso o usuário pressione uma chave, um LED deve piscar a cada 1 segundo.

; -------------------------------------------------------------------------------
        ;THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; ========================
; Definições de Valores
BIT0	EQU 0001b
BIT1	EQU 0010b

GPIO_PORTL_AHB_LOCK_R    	EQU    0x40062520
GPIO_PORTL_AHB_CR_R      	EQU    0x40062524
GPIO_PORTL_AHB_AMSEL_R   	EQU    0x40062528
GPIO_PORTL_AHB_PCTL_R    	EQU    0x4006252C
GPIO_PORTL_AHB_DIR_R     	EQU    0x40062400
GPIO_PORTL_AHB_AFSEL_R   	EQU    0x40062420
GPIO_PORTL_AHB_DEN_R     	EQU    0x4006251C
GPIO_PORTL_AHB_PUR_R     	EQU    0x40062510	
GPIO_PORTL_AHB_DATA_R    	EQU    0x400623FC
GPIO_PORTL               	EQU    000010000000000b

GPIO_PORTM_AHB_LOCK_R    	EQU    0x40063520
GPIO_PORTM_AHB_CR_R      	EQU    0x40063524
GPIO_PORTM_AHB_AMSEL_R   	EQU    0x40063528
GPIO_PORTM_AHB_PCTL_R    	EQU    0x4006352C
GPIO_PORTM_AHB_DIR_R     	EQU    0x40063400
GPIO_PORTM_AHB_AFSEL_R   	EQU    0x40063420
GPIO_PORTM_AHB_DEN_R     	EQU    0x4006351C
GPIO_PORTM_AHB_PUR_R     	EQU    0x40063510	
GPIO_PORTM_AHB_DATA_R    	EQU    0x400633FC
GPIO_PORTM               	EQU    000100000000000b	; porta 11


; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis

                SECTION .data : DATA (2)
                //AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código

        SECTION .text : CODE (2)
        //AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
;        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
             EXPORT Start
             
 ;DEVE-SE COLOCAR O PINO PL4 E O PM4 COMO DIGITAL MAS COMO InPUT

Start
		PUSH {R4,R5,R6,R7}
                cpsid i
                MOV             R6, R0
                MOV             R7, R1
		MOV       	R5,#0x00000000

Repet_Cycle

		MOV		R0,#0x00000000
		MOV       	R1,#0x00000000
      
      
Is_Down
		LDR		R2, =GPIO_PORTL_AHB_DATA_R
		LDR       	R3, [R2]
		BIC       	R3,R3, #11101111b
		CMP       	R3, #0x10
      
		BNE       Is_Down
      
Is_Up
		LDR		R2, =GPIO_PORTL_AHB_DATA_R
		LDR       	R3, [R2]
		BIC       	R3,R3, #11101111b
		CMP       	R3, #0x10
		BEQ       	Is_Up
      
      
Down_Cycle
      
		ADD       	R0,R0,#12
		LDR		R2, =GPIO_PORTL_AHB_DATA_R
		LDR       	R3, [R2]
		BIC       	R3,R3, #11101111b
		CMP       	R3, #0x10
      
		BNE       Down_Cycle
      
Up_Cycle

		ADD       	R1,R1,#12
		LDR		R2, =GPIO_PORTL_AHB_DATA_R
		LDR       	R3, [R2]
		BIC       	R3,R3, #11101111b
		CMP       	R3, #0x10
		
		BEQ       Up_Cycle

		PUSH      	{R0,R1}
		ADD       	R5, R5, #1
		CMP       	R5, #3
      
		BNE       Repet_Cycle
                
		MOV       	R2,#0x00000000
		MOV       	R3,#0x00000000
      
      
		POP       	{R0,R1}
		ADD       	R2,R0
		ADD       	R3,R1
      
		POP       	{R0,R1}
		ADD       	R2,R0
		ADD       	R3,R1
      
		POP       	{R0,R1}
		ADD       	R2,R0
		ADD       	R3,R1
      
      
		MOV       	R4,#3
      
		UDIV      	R0,R2,R4
		UDIV      	R1,R3,R4
                STR             R0,[R6]
                STR             R1,[R7]
                
                cpsie i
                POP             {R4,R5,R6,R7}
		BX      	LR
	
                //ALIGN                           ; garante que o fim da seção está alinhada 
                END                             ; fim do arquivo