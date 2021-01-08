/*
 * ConsoleComand.h
 *
 *  Created on: Jan 4, 2021
 *      Author: tabur
 */

#ifndef INC_CONSOLECOMMAND_H_
#define INC_CONSOLECOMMAND_H_

#include "main.h"
#include <stdio.h>
#include <string.h>

#define CONSOLE_COMMAND() \
	void ConsoleCommand(uint8_t* string)

typedef void (*ConsoleCommandHandler)(uint8_t* string);

CONSOLE_COMMAND();

HAL_StatusTypeDef ConsoleStart(UART_HandleTypeDef *huart);
HAL_StatusTypeDef ConsoleStop(void);

#endif /* INC_CONSOLECOMMAND_H_ */
