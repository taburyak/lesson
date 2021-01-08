/*
 * ConsoleCommand.c
 *
 *  Created on: Jan 4, 2021
 *      Author: tabur
 */
#include "ConsoleCommand.h"

#define MAXCLISTRING          100 // Biggest string the user will type

UART_HandleTypeDef *xHuart;

uint8_t rxBuffer = '\000'; 		// where we store that one character that just came in
uint8_t rxString[MAXCLISTRING]; // where we build our string from characters coming in
int rxindex = 0; 				// index for going though rxString

__attribute__ ((weak)) void ConsoleCommand(uint8_t* string) { UNUSED(string); }
static void ExecuteSerialCommand(uint8_t* string);

HAL_StatusTypeDef ConsoleStart(UART_HandleTypeDef *huart)
{
	xHuart = huart;
	return HAL_UART_Receive_DMA(xHuart, &rxBuffer, 1);
}

HAL_StatusTypeDef ConsoleStop(void)
{
	return HAL_UART_DMAStop(xHuart);
}

int _write(int file, char *ptr, int len)
{
	//HAL_UART_DMAStop(xHuart);
	HAL_UART_Transmit(xHuart, (uint8_t *) ptr, len, HAL_MAX_DELAY);
	//HAL_UART_Transmit_DMA(xHuart, (uint8_t *) ptr, len);
	return len;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == xHuart->Instance)
	{
		//HAL_UART_Receive_DMA(xHuart, &rxBuffer, 1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == xHuart->Instance)
	{
		int i = 0;

		printf((char*) &rxBuffer); // Echo the character that caused this callback so the user can see what they are typing

		if (rxBuffer == 8 || rxBuffer == 127) // If Backspace or del
		{
			printf(" \b"); // "\b space \b" clears the terminal character. Remember we just echoced a \b so don't need another one here, just space and \b
		    rxindex--;

		    if (rxindex < 0)
		    {
		    	rxindex = 0;
		    }
		 }
		 else if (rxBuffer == '\n' || rxBuffer == '\r') // If Enter
		 {
			 ExecuteSerialCommand(rxString);
		     rxString[rxindex] = 0;
		     rxindex = 0;

		     for (i = 0; i < MAXCLISTRING; i++)
		     {
		    	 rxString[i] = 0; // Clear the string buffer
		     }
		 }
		 else
		 {
			 rxString[rxindex] = rxBuffer; // Add that character to the string
		     rxindex++;

		     if (rxindex > MAXCLISTRING) // User typing too much, we can't have commands that big
		     {
		    	 rxindex = 0;
		         for (i = 0; i < MAXCLISTRING; i++)
		         {
		        	 rxString[i] = 0; // Clear the string buffer
		         }
		     }
		 }
		//HAL_UART_Receive_DMA(xHuart, &rxBuffer, 1);
	}
}

static void ExecuteSerialCommand(uint8_t* string)
{
	ConsoleCommandHandler ConsoleHandler = ConsoleCommand;
	ConsoleHandler(string);
}
