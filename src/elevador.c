#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "system_tm4c1294.h"
#include "cmsis_os2.h"

// includes do driverlib
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
void UART_Write(char *instr);

uint16_t Flag = 0x0001;
char instr[6];
char instr1[6];
char instr2[6];
char instr3[6];


osThreadId_t elevador_left_id, elevador_mid_id, elevador_right_id, UART_Read_id;
osTimerId_t timer_left_id, timer_mid_id, timer_right_id;
osMutexId_t Mutex_UART;

typedef struct {
  int andar;
  char pos; 
  int status; // 1 = parado, 2 = subindo, 3 = descendo
  bool paradas[16];
}struct_elevador;

extern void UARTStdioIntHandler(void);

void callback(void *arg){} 
char andar_numb(int letra_andar)
{
 char numb;
  switch(letra_andar)
  {
  case 0:
    {numb='a';break;}
  case 1:
    {numb='b';break;}
  case 2:
    {numb='c';break;}
  case 3:
    {numb='d';break;}
  case 4:
    {numb='e';break;}
  case 5:
    {numb='f';break;}
  case 6:
    {numb='g';break;}
  case 7:
    {numb='h';break;}
  case 8:
    {numb='i';break;}
  case 9:
    {numb='j';break;}
  case 10:
    {numb='k';break;}
  case 11:
    {numb='l';break;}
  case 12:
    {numb='m';break;}
  case 13:
    {numb='n';break;}
  case 14:
    {numb='o';break;}
  case 15:
    {numb='p';break;}
  }
  return numb;
}

void botao_on(char posicao,int numb_andar)
{
  char str[5];
      str[0]=posicao;
      str[1]='L';
      str[2]=andar_numb(numb_andar);
      str[3]='\n';
      osMutexAcquire(Mutex_UART, osWaitForever);
      UARTprintf("%s\r",str);
      osMutexRelease(Mutex_UART);
      
}
void botao_off(char posicao,int numb_andar)
{
  char str[5];
      str[0]=posicao;
      str[1]='D';
      str[2]=andar_numb(numb_andar);
      str[3]='\n';
      osMutexAcquire(Mutex_UART, osWaitForever);
      UARTprintf("%s\r",str);
      osMutexRelease(Mutex_UART);
}

int conv_letra_andar(char andar_numb)
{
  int andar;
  switch(andar_numb)
  {
  case 'a':
    {andar=0;break;}
  case 'b':
    {andar=1;break;}
  case 'c':
    {andar=2;break;}
  case 'd':
    {andar=3;break;}
  case 'e':
    {andar=4;break;}
  case 'f':
    {andar=5;break;}
  case 'g':
    {andar=6;break;}
  case 'h':
    {andar=7;break;}
  case 'i':
    {andar=8;break;}
  case 'j':
    {andar=9;break;}
  case 'k':
    {andar=10;break;}
  case 'l':
    {andar=11;break;}
  case 'm':
    {andar=12;break;}
  case 'n':
    {andar=13;break;}
  case 'o':
    {andar=14;break;}
  case 'p':
    {andar=15;break;}
  }
  return andar;
}

int conv_num_andar(char dezena, char unidade){
      return (dezena-0x30)*10+(unidade-0x30);  
}


int get_andar(char dezena, char unidade){
    if(unidade=='\0'){
      return   (dezena-0x30);
    }
     return (dezena-0x30)*10+(unidade-0x30);
}

void prencher_parada(struct_elevador* e,int andar_destino){
   e->paradas[andar_destino]=true;
}

void retirar_parada(struct_elevador* e){
  e->paradas[e->andar]=false;
}


bool esvaziou_parada(bool*vet){
  for(int i=0;i<=15;i++)
  {
    if(vet[i]==true)
    {return false;}
  }
  return true;
}

bool comparar_andar(int andar_atual,bool *vet){
    if(vet[andar_atual]==true)
    {
      return true;
    }
    else{return false;}
  }

void reset(char pos){
    char send[2];
    send[0]=pos;
    send[1]='r';
    UART_Write(send);
}

void abrir(char pos){
    char send[2];
    send[0]=pos;
    send[1]='a';//abrir
    UART_Write(send);
    osDelay(1500);
}

void fechar(char pos){
    char send[2];
    send[0]=pos;
    send[1]='f';//fechar
    UART_Write(send);
    osDelay(1500);
}

void parar(char pos){
  char send[2];
  send[0]=pos;
  send[1]='p';//parar
  UART_Write(send);
  osDelay(1500);
}

void subir(char pos){
  char send[2];
  send[0]=pos;
  send[1]='s';//subir
  UART_Write(send);
  
}

void descer(char pos){
  char send[2];
  send[0]=pos;
  send[1]='d';//descer
  UART_Write(send);
}

void receber(struct_elevador e){
    parar(e.pos);
    osDelay(1500);
    abrir(e.pos);
    osDelay(1500);
}

int num(bool *vet)
{
  int cont=0;
  for(int i=0;i<=15;i++)
  {     
    if(vet[i]==true)
    {
        cont++;
    }
  }
  return cont;
}

bool paradas_acima(struct_elevador e){
  
  for(int i=0;i<=15;i++)
  {
    if(i>e.andar)
    {
      if(e.paradas[i]==true)
      {
        return true;
      }
    }
  }
  return false;
}


bool paradas_abaixo(struct_elevador e){
  for(int i=0;i<=15;i++)
  {
    if(i<e.andar)
    {
      if(e.paradas[i]==true)
      {
        return true;
      }
    }
  }
  return false;
}


// ***************** Funções e Configuração da UART ************

void UART_Read(void *arg){
  
  while(1){
    UARTgets(instr,20); //6
    
    if(instr[0]=='e'){
      for(int i=0;i<=6;i++)
      {instr1[i]=instr[i];}
      osThreadFlagsSet(elevador_left_id,Flag);
    }
    else if(instr[0]=='c'){
      for(int i=0;i<=6;i++)
      {instr2[i]=instr[i];}
      osThreadFlagsSet(elevador_mid_id,Flag);
    }
    else if(instr[0]=='d'){
      
      for(int i=0;i<=6;i++)
      {instr3[i]=instr[i];}
      osThreadFlagsSet(elevador_right_id,Flag);
    }  
    osThreadYield();
  } 
} 

void UART_Write(char *instr){
    osMutexAcquire(Mutex_UART, osWaitForever);
    UARTprintf("%c%c\n\r",instr[0],instr[1]); 
    osMutexRelease(Mutex_UART);
   // osDelay(500);
}

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
  UARTStdioConfig(0,115200, SystemCoreClock);
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler


// ****************** Função das Threads do Elevador **************

void elevador(void *arg){
	
  int andar_destino;
  
  struct_elevador e;
  e.andar=0;
  e.status=1;
  e.pos=(char)(arg);
  char *istr;
  for(int i=0;i<=15;i++)
  {e.paradas[i]=false;
  }
  
  if(e.pos=='e')
  {istr=instr1;}
  if(e.pos=='c')
  {istr=instr2;}
  if(e.pos=='d')
  {istr=instr3;}
  
  
  reset(e.pos);
  
  
  while(1){
	  
   osThreadFlagsWait(Flag, osFlagsWaitAny, 500);
	
  if(istr[1]>='0' && istr[1]<='9')
    {
        e.andar=get_andar(istr[1],istr[2]);
        if(comparar_andar(e.andar, e.paradas)) //andar de destino
        {
            receber(e);
            retirar_parada(&e);
            botao_off(e.pos,e.andar);
            
            if(esvaziou_parada(e.paradas))//true
            {
               parar(e.pos);
               e.status=1;
            }
            else if(e.status==2) // continua subindo
            {
                if(paradas_acima(e)){
                    fechar(e.pos);
                    subir(e.pos);
                }
                else // pode inverter o direcao
                {
                    e.status=3;
                    fechar(e.pos);
                    descer(e.pos);
                }
            }
            else if(e.status==3) // continua descendo
            {
                  if(paradas_abaixo(e)){
                      fechar(e.pos);
                      descer(e.pos);
                  }
                  else // pode inverter o direcao
                  {
                      e.status=2;
                      fechar(e.pos);
                      subir(e.pos);
                  }
             }
            
         }
          else//false
          {
              if(e.status==2){
                  subir(e.pos);
              }
              else if(e.status==3){
                descer(e.pos);
              }
          }
        for(int i=0;i<=6;i++)
        {istr[i]='x';}
    }
    else if(istr[1]=='E')// botão solicitando subir ou descer em cada andar
    {
      andar_destino = conv_num_andar(istr[2],istr[3]);
      prencher_parada(&e, andar_destino);
      if(e.status == 1)//PARADO
      {
        if(andar_destino == e.andar){
            receber(e);
        }
        if(andar_destino > e.andar){
            fechar(e.pos);
            subir(e.pos);
            e.status=2;
            botao_on(e.pos,andar_destino);
        }
        else{
            fechar(e.pos);
            descer(e.pos);
            e.status=3;
            botao_on(e.pos,andar_destino);
        }
      }
      else
      {
        botao_on(e.pos,andar_destino);
        if(andar_destino > e.andar){
            subir(e.pos);
            e.status=2;
            botao_on(e.pos,andar_destino);
        }
        else if (andar_destino > e.andar){
            descer(e.pos);
            e.status=3;
            botao_on(e.pos,andar_destino);
        }
      }
     for(int i=0;i<=6;i++)
      {istr[i]='x';}
    }
    else if(istr[1]=='I'){
      andar_destino=conv_letra_andar(istr[2]);
      prencher_parada(&e, andar_destino);
      if(e.status==1)
      {
        if(andar_destino>e.andar){
            fechar(e.pos);
            botao_on(e.pos,andar_destino);
            subir(e.pos);
            e.status=2;
        }
        else if(andar_destino<e.andar){
            fechar(e.pos);
            botao_on(e.pos,andar_destino);
            descer(e.pos);
            e.status=3; 
        }
        else{
            receber(e);
              
        }
      }
      else if(e.status==2)
      {
        subir(e.pos);
        botao_on(e.pos,andar_destino);
      }
      else if(e.status==3)
      {
        descer(e.pos);
        botao_on(e.pos,andar_destino);
      }
      for(int i=0;i<=6;i++)
      {istr[i]='x';}
    }
    if(e.status==1)
    {
      if(esvaziou_parada(e.paradas)==false)
      {
        if(paradas_acima(e))
        {
            fechar(e.pos);
            subir(e.pos);
            e.status=2;
        }
        else if(paradas_abaixo(e))
        {
            fechar(e.pos);
            subir(e.pos);
            e.status=3;
        }
      
      }
    
    }
    
  } 
}

void main(void){
  
  UARTInit();
  
  
  SystemInit();
 
  osKernelInitialize();

  elevador_left_id = osThreadNew(elevador, (void*)'e', NULL);
  elevador_mid_id = osThreadNew(elevador, (void*)'c', NULL);
  elevador_right_id = osThreadNew(elevador, (void*)'d', NULL);
  
  Mutex_UART = osMutexNew(NULL);
  
  UART_Read_id= osThreadNew(UART_Read, NULL, NULL);

  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
}