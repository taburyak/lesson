/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ConsoleCommand.h"
#include <string.h>
/* USER CODE END Includes */

/* USER CODE BEGIN 2 */
  if (ConsoleStart(&huart2) == HAL_OK)
  {
	  printf("Start receive console command:\r\n");
  }
  else
  {
	  printf("Error USART:\r\n");
  }
  /* USER CODE END 2 */
  
  /* USER CODE BEGIN 4 */
CONSOLE_COMMAND()
{
	printf("\r\nConsole > ");

	if (strcmp ((char*) string, "at") == 0)
	{
		printf("OK\r\n");
	}
	else if (strcmp((char*) string, "on") == 0)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, ENABLE);
		printf("LED is ON\r\n");
	}
	else if (strcmp((char*) string, "off") == 0)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, DISABLE);
		printf("LED is OFF\r\n");
	}
	else if (strcmp((char*) string, "stop") == 0)
	{
		printf("Stop receive command\r\n");
		ConsoleStop();
	}
	else if (strcmp((char*) string, "reset") == 0)
	{
		printf("Reset Device now!\r\n");
		HAL_NVIC_SystemReset();
	}
	else if (strcmp((char*) string, "id") == 0)
	{
		printf("ID device: %ld\r\n", HAL_GetDEVID());
	}
	else if (strcmp((char*) string, "version") == 0)
	{
		printf("HAL version: %ld\r\n", HAL_GetHalVersion());
	}
	else if (strcmp((char*) string, "status") == 0)
	{
		if (HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin))
		{
			printf("Status led ON\r\n");
		}
		else
		{
			printf("Status led OFF\r\n");
		}
	}
	else
	{
		printf("Unknown command\r\n");
	}
}
/* USER CODE END 4 */