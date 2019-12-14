// Victor Hugo S. dos Reis e Doglas Querino
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

#include "system_TM4C1294.h" 

uint8_t LED_D1 = 0;
//uint8_t freq = 0;
//uint8_t pulse_width = 0;
//uint8_t periodo = 0;
//uint8_t duty_cycle = 0;
//uint8_t aux = 0;
//uint16_t cont = 0;
static float freq = 0;
static float pulse_width = 0;
static float periodo = 0;
static float duty_cycle = 0;
static uint8_t aux = 0;
//float cont = 0;

extern void UARTStdioIntHandler(void);



// Configuração da UART *********

void UARTInit(void){
  // Enable the GPIO Peripheral used by the UART.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  // Enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, 9600, SystemCoreClock);
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

// ******************************


/*
void SysTick_Handler(void){
  LED_D1 ^= GPIO_PIN_1; // Troca estado do LED D1
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LED_D1); // Acende ou apaga LED D1
} // SysTick_Handler
*/


void main(void){
  
  UARTInit();
  UARTprintf("Teste\n");
  
  /*
  float freq;
  float pulse_width = 0;
  float periodo = 0;
  float duty_cycle = 0;
  int aux = 0;
  static uint32_t cont = 0;
*/
  static float cont = 0;
  static char sfreq[20];
  static char speriodo[20];
  static char sduty_cycle[20];

  
  //SysTickPeriodSet(12000000); // f = 1Hz para clock = 24MHz

  
  // Configuração dos LEDs e push-buttons
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // Habilita GPIO N (LED D1 = PN1, LED D2 = PN0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)); // Aguarda final da habilitação
  
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); // LEDs D1 e D2 como saída
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0); // LEDs D1 e D2 apagados
  GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D3 = PF4, LED D4 = PF0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); // LEDs D3 e D4 como saída
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0); // LEDs D3 e D4 apagados
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL); // GPIO L

  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)); // Aguarda final da habilitação

  
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // push-buttons SW1 e SW2 como entrada
  GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_4); // pin L4 como entrada

  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  // ************************************

  
  //SysTickIntEnable();
  //SysTickEnable();
  
  //aux = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4);
  while(1){
    if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4) == 16) // Testa estado do pino do gerador até ser alto
      break;
   }
  
  while(1){
    /*
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == GPIO_PIN_0) // Testa estado do push-button SW1
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0); // Apaga LED D3
    else
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4); // Acende LED D3

    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == GPIO_PIN_1) // Testa estado do push-button SW2
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0); // Apaga LED D4
    else
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0); // Acende LED D4
    */
    
    cont = 0.;
    
    aux = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4);
    while(1){
    if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4) != aux) // Testa estado do pino do gerador até ser baixo
      break;
    else
      cont = cont + 1;
   }
   pulse_width =(float) cont/24000000;
   
   //aux = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4);
   while(1){
    if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4) == 16) // Testa estado do pino do gerador até ser alto
      break;
    else
      cont++;
   }
   periodo =(float) cont/SystemCoreClock;
   
   freq =(float) 1/cont;
   duty_cycle = (pulse_width/periodo)*100;
   /*
   UARTprintf("freq = %f\n", freq);
   UARTprintf("periodo = %f\n", periodo);
   UARTprintf("duty_cycle = %f\n\n", duty_cycle);
   */
   sprintf(sfreq, "%f\n",freq);
   UARTprintf("freq = %s\n",sfreq);
   sprintf(speriodo, "%f\n",periodo);
   UARTprintf("periodo = %s\n", speriodo);
   sprintf(sduty_cycle, "%f\n",duty_cycle);
   UARTprintf("duty_cycle = %s\n\n", sduty_cycle);
  } // while
} // main