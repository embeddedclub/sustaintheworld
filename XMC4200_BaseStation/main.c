/*
 * Copyright (C) 2019 Infineon Technologies AG. All rights reserved.
 *
 * Infineon Technologies AG (Infineon) is supplying this software for use with
 * Infineon's microcontrollers.
 * This file can be freely distributed within development tools that are
 * supporting such microcontrollers.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

/**
 * @file
 * @date 29 May, 2019
 * @version 1.0.0
 *
 * @brief XMC4300 EtherCAT Relax kit external triggered SPI transfer
 *
 * This examples shows how to trigger externally an SPI word transfer
 *
 * History <br>
 *
 * Version 1.0.0 Initial <br>
 *
 */


#include <xmc_common.h>
#include <xmc_gpio.h>
#include <xmc_uart.h>
#include "spi_drv.h"
#include "tle94112.h"



/* Pins macro definition */

#define TICKS_PER_SECOND 1000
#define TICKS_WAIT 1000

#define LED1 P0_1
#define HB_EN P2_6
#define LOAD_IN4 P2_2
#define LOAD_IN3 P1_9
#define LOAD_IN2 P1_7
#define LOAD_IN1 P0_11

#define UART_TX P1_5
#define UART_RX P2_15

/* Number of data to tx/rx and limits for the TX and RX FIFOs */

#define NUM_DATA 255
#define RX_FIFO_INITIAL_LIMIT 7
#define TX_FIFO_INITIAL_LIMIT 1

/* Initialization structures */

XMC_GPIO_CONFIG_t uart_tx =
{
  .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM
};

XMC_GPIO_CONFIG_t uart_rx =
{
  .mode = XMC_GPIO_MODE_INPUT_TRISTATE
};

/* Initialization structure for both USIC0_CH0 and USIC0_CH1 channels */
XMC_UART_CH_CONFIG_t uart_config =
{
  .data_bits = 8U,
  .stop_bits = 1U,
  .baudrate = 921600U //115200U
};

/* Global variables initialization. Tx and rx indexes and tx and rx arrays */
uint32_t tx_index=0;
uint32_t rx_index=0;
uint8_t tx_data[NUM_DATA];
uint8_t rx_data[NUM_DATA];

uint8_t data[8] = {1,2,3,4,5,6,7,8};
uint8_t app_data[6];
uint8_t pwm_count =0;

uint8_t setup =0;
volatile uint8_t data_found =0;

volatile uint8_t data_avilable =0;

void delay(uint32_t dly)
{

	while(dly--);
}

void clean_rxBuffer()
{
	uint8_t i;

	for(i=0;i<NUM_DATA;i++)
	{

		rx_data[i]='0';
	}

}

void SysTick_Handler(void)
{
  static uint32_t ticks = 0;

  ticks++;

  if (ticks == TICKS_WAIT)
  {

    XMC_GPIO_ToggleOutput(LED1);


    if(app_data[5]=='1')
	{
        XMC_GPIO_ToggleOutput(LOAD_IN4);
	}
    else
    {
    	XMC_GPIO_SetOutputLow(LOAD_IN4);
    }



	if(app_data[1]=='1')
	{
		XMC_GPIO_SetOutputHigh(LOAD_IN1);;
	}
	else
	{
		XMC_GPIO_SetOutputLow(LOAD_IN1);
	}

	if(app_data[3]=='1')
	{
		XMC_GPIO_SetOutputHigh(LOAD_IN2);
	}
	else
	{
		XMC_GPIO_SetOutputLow(LOAD_IN2);
	}

    ticks = 0;

  }
}


/* Transmit handling IRQ */
void USIC0_0_IRQHandler(void)
{
  /* If still remaining data to be send */
  if(tx_index < NUM_DATA)
  {
    /* Fill the TX FIFO till it is full */
  //  while(!XMC_USIC_CH_TXFIFO_IsFull(XMC_UART0_CH0))
  //  {
//	  XMC_UART_CH_Transmit(XMC_UART0_CH0, tx_data[tx_index]);
//	  tx_index++;
//    }
  }
}

/* Receive handling IRQ */
void USIC1_1_IRQHandler(void)
{

  /* Read the RX FIFO till it is empty */
  while(!XMC_USIC_CH_RXFIFO_IsEmpty(XMC_UART1_CH0))
  {

    rx_data[rx_index] = XMC_UART_CH_GetReceivedData(XMC_UART1_CH0);
  //	XMC_UART_CH_Transmit(XMC_UART0_CH0,rx_data[rx_index]);
    if((rx_data[rx_index]=='#') && !data_found)
	{

		data_found = 1;
		XMC_UART_CH_Transmit(XMC_UART0_CH0,'\n');
	}

    if((rx_data[rx_index]=='#') && data_found && rx_index !=0)
	{

		data_found = 0;
		XMC_UART_CH_Transmit(XMC_UART0_CH0,'\n');
		rx_index=0;
		memcpy(app_data,&rx_data[10],6);
		clean_rxBuffer();
		data_avilable++;
	}

    if(data_found == 1){
    	XMC_UART_CH_Transmit(XMC_UART0_CH0,rx_data[rx_index]);
    	rx_index++;

    }
    else
    {
    	rx_index=0;
    }
  }
#if 1
  /* If all the data have been received */
  if(rx_index == (NUM_DATA))
  {
	  rx_index = 0;

  }

  /* If the remaining data to be received is smaller than the initial RX FTFO Limit
   * it is modified to the remaining data minus 1 in order react when we have all the data received*/
  if((NUM_DATA-rx_index) < RX_FIFO_INITIAL_LIMIT)
  {
    XMC_USIC_CH_RXFIFO_SetSizeTriggerLimit(XMC_UART1_CH0, XMC_USIC_CH_FIFO_SIZE_8WORDS, (NUM_DATA-rx_index)-1);
  }

#endif
}


void init_uart()
{
	 /* USIC channels initialization */
	  XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);
	  XMC_UART_CH_Init(XMC_UART1_CH0, &uart_config);

	  /* Input multiplexer configuration for UART1_CH0 */
	  XMC_UART_CH_SetInputSource(XMC_UART1_CH0, XMC_UART_CH_INPUT_RXD, USIC1_C0_DX0_P2_15);

	  /* FIFOs initialization for both channels:
	   *  8 entries for TxFIFO from point 0, LIMIT=1
	   *  8 entries for RxFIFO from point 8, LIMIT=7 (SRBI is set if all 8*data has been received)
	   *  */
	  XMC_USIC_CH_TXFIFO_Configure(XMC_UART0_CH0, 0, XMC_USIC_CH_FIFO_SIZE_8WORDS, TX_FIFO_INITIAL_LIMIT);
	  XMC_USIC_CH_RXFIFO_Configure(XMC_UART1_CH0, 8, XMC_USIC_CH_FIFO_SIZE_8WORDS, RX_FIFO_INITIAL_LIMIT);

	  /* Enabling events for TX FIFO and RX FIFO */
	  XMC_USIC_CH_TXFIFO_EnableEvent(XMC_UART0_CH0,XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
	  XMC_USIC_CH_RXFIFO_EnableEvent(XMC_UART1_CH0,XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD |
			                                       XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);

	  /* Connecting the previously enabled events to a Service Request line number */
	  XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(XMC_UART0_CH0,XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD,0);
	  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_UART1_CH0,XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD,1);
	  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(XMC_UART1_CH0,XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE,1);

	  /* Start USIC operation as UART */
	  XMC_UART_CH_Start(XMC_UART0_CH0);
	  XMC_UART_CH_Start(XMC_UART1_CH0);

	  /*Initialization of the necessary ports*/
	  XMC_GPIO_Init(UART_TX,&uart_tx);
	  XMC_GPIO_Init(UART_RX,&uart_rx);
}


int main(void)
{
  XMC_GPIO_CONFIG_t config;

  config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
  config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
  config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;

  XMC_GPIO_Init(LED1, &config);
 // XMC_GPIO_Init(HB_EN,&config);
  XMC_GPIO_Init(LOAD_IN4,&config);
  XMC_GPIO_Init(LOAD_IN1,&config);
  XMC_GPIO_Init(LOAD_IN2,&config);


  /* System timer configuration */
  SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

  //spi_init();

 // XMC_GPIO_SetOutputHigh(HB_EN);
#if 0
  tle_init();
  tle_configHB_LS12(1,1);
  tle_setpwmModFreq(0, 0, 3);
  tle_setpwm1(100);
#endif

  init_uart();
#if 0
    if(setup == 0)
    {
  	  tle_configHB_LS12(1,1);
  	  tle_setpwmModFreq(0, 0, 3);
  	  tle_setpwmModFreq(0, 0, 3);
  	  tle_setpwm1(20);
	  setup = 1;
	  pwm_count = 20;
    }
#endif

    /*Configuring priority and enabling NVIC IRQ for the defined Service Request line number */
    NVIC_SetPriority(USIC0_0_IRQn,63U);
    NVIC_EnableIRQ(USIC0_0_IRQn);
    NVIC_SetPriority(USIC1_1_IRQn,62U);
    NVIC_EnableIRQ(USIC1_1_IRQn);

 // tle_configHB1LS(1,0,0,0);
 // tle_configHB2LS(1,0,0,0);
 // tle_configHB3(1, 0, 0, 0);


  //tle_config1(50);
 //   XMC_UART_CH_Transmit(XMC_UART0_CH0,'E');
 //   XMC_UART_CH_Transmit(XMC_UART0_CH0,'A');
 //   XMC_UART_CH_Transmit(XMC_UART0_CH0,'\n');
  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {

  }
}
