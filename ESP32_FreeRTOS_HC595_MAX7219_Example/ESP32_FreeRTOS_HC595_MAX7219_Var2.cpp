#include <Arduino.h>
#include "max7219.h"
#include <SPI.h>

#define CS_HC595_PIN    5
#define CS_MAX7219_PIN  15
#define KEY_PIN         0
#define LED_BLUE_PIN    2

#define CS_HC595_SET()  digitalWrite(CS_HC595_PIN, LOW)
#define CS_HC595_RESET()  digitalWrite(CS_HC595_PIN, HIGH)

Max7219 display = Max7219(CS_MAX7219_PIN, 8, 5);

SemaphoreHandle_t mutexSPI;
SemaphoreHandle_t mutexSerial;

TaskHandle_t Task2Handle = NULL;

void Task1(void* parameters);
void Task2(void* parameters);
void Task3(void* parameters);
void Task4(void* parameters);
void Task5(void* parameters);

void setup() 
{
  Serial.begin(115200);
  SPI.begin(18, 21, 23);

  mutexSPI = xSemaphoreCreateMutex();
  mutexSerial = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(Task1, "Task1", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task2, "Task2", 2048, NULL, 1, &Task2Handle, 1);
  xTaskCreate(Task3, "Task3", 2048, NULL, 4, NULL);
  xTaskCreate(Task4, "Task4", 2048, NULL, 1, NULL);
  xTaskCreate(Task5, "Task5", 2048, NULL, 1, NULL);
}

void loop() 
{
  
}

void Task1(void* parameters)
{
  // code runing only once
  pinMode(CS_HC595_PIN, OUTPUT);
  CS_HC595_RESET();

  while(1)
  {
    //code runing infinite
    for (uint8_t i = 0; i < 8; i++)
    {
      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] Run Task1 on Core: %d\r\n", millis(), xPortGetCoreID());
      xSemaphoreGive(mutexSerial);
      
      xSemaphoreTake(mutexSPI, portMAX_DELAY);
      CS_HC595_SET();
      SPI.transfer(1 << i);
      CS_HC595_RESET();
      xSemaphoreGive(mutexSPI);
      vTaskDelay(444 / portTICK_PERIOD_MS);
    }
  }
}

void Task2(void* parameters)
{
  display.Begin();

  while (1)
  {
    display.Clean();
    display.DecodeOn();

    for (uint8_t i = 0; i < 8; i++)
    {
      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] Run Task2 on Core: %d\r\n", millis(), xPortGetCoreID());
      xSemaphoreGive(mutexSerial);
      
      xSemaphoreTake(mutexSPI, portMAX_DELAY);
      display.PrintNtos(8, (uint32_t) millis(), 8);      
      xSemaphoreGive(mutexSPI);

      vTaskDelay(993 / portTICK_PERIOD_MS);
    }
  }
  
}

void Task3(void* parameters)
{
  pinMode(KEY_PIN, INPUT);
  bool keyFlag = false;
  bool task2Activity = true;

  while (1)
  {
    if (!digitalRead(KEY_PIN) && !keyFlag)
    {
      keyFlag = true;

      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] Key is pressed on Core: %d\r\n", millis(), xPortGetCoreID());
      xSemaphoreGive(mutexSerial);
    }
    
    if (digitalRead(KEY_PIN) && keyFlag)
    {
      keyFlag = false;

      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] Key is unpressed on Core: %d\r\n", millis(), xPortGetCoreID());
      xSemaphoreGive(mutexSerial);

      if (task2Activity)
      {
        task2Activity = false;
        vTaskSuspend(Task2Handle);
      }
      else
      {
        task2Activity = true;
        vTaskResume(Task2Handle);
      }
      
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  
}

void Task4(void* parameters)
{
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_BLUE_PIN, 0);

  uint8_t n = 0;
  bool direction = false;

  while (1)
  {
    if (!direction && n == 255)
    {
      direction = true;
    }

    if (direction && n == 0)
    {
      direction = false;
    }
    
    ledcWrite(0, n);

    if (!direction)
    {
      n++;
    }
    else
    {
      n--;
    }
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  
}

void Task5(void* parameters)
{
  uint8_t brightness[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                          15, 14, 13, 12, 11, 10 , 9, 8, 7, 6, 5, 4, 3, 2, 1};
  
  while (1)
  {
    for (uint8_t i = 0; i < sizeof(brightness) / sizeof(brightness[0]); i++)
    {
      xSemaphoreTake(mutexSPI, portMAX_DELAY);
      display.SetIntensivity(brightness[i]);
      xSemaphoreGive(mutexSPI);

      vTaskDelay(111 / portTICK_PERIOD_MS);
    }
  }
}
