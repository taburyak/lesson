#include <Arduino.h>
#include <SPI.h>
#include "max7219.h"

#define CS_HC595    5
#define CS_MAX7219  15
#define KEY_PIN     0
#define LED_BLUE    2

#define CS_HC595_SET()  digitalWrite(CS_HC595, LOW)
#define CS_HC595_RESET()  digitalWrite(CS_HC595, HIGH)

void Task0(void* parameters);
void Task1(void* parameters);
void Task2(void* parameters);
void Task3(void* parameters);
void Task4(void* parameters);
void Task5(void* parameters);

bool keyFlag = false;
bool task4Active = true;

TaskHandle_t Task2Handle = NULL;

SemaphoreHandle_t mutexSPI;
SemaphoreHandle_t mutexSerial;

Max7219 max7219 = Max7219(CS_MAX7219, 8, 9);

void setup() 
{
  Serial.begin(115200);
  Serial.println("Serial is OK!");
  SPI.begin(18, 21, 23);
  max7219.Begin();

  mutexSPI = xSemaphoreCreateMutex();
  mutexSerial = xSemaphoreCreateMutex();

  xTaskCreate(Task0, "Task0", 2048, NULL, 1, NULL);
  xTaskCreatePinnedToCore(Task1, "Task1", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task2, "Task2", 2048, NULL, 1, &Task2Handle, 1);
  xTaskCreate(Task3, "Task3", 2048, NULL, 2, NULL);
  xTaskCreate(Task4, "Task4", 2048, NULL, 1, NULL);
  xTaskCreate(Task5, "Task5", 2048, NULL, 1, NULL);
}

void loop() 
{
  
}

void Task0(void* parameters)
{
  xSemaphoreTake(mutexSerial, portMAX_DELAY);
  Serial.printf("[%8lu] Run Task0 only once: %d\r\n", millis(), xPortGetCoreID());
  xSemaphoreGive(mutexSerial);

  vTaskDelete(NULL);
}

void Task1(void* parameters)
{
  //TODO: код що виконується раз при запускі задачі
  pinMode(CS_HC595, OUTPUT);
  CS_HC595_RESET();
  
  while (1)
  {
    //TODO: код що виконується безкінечно
    for (size_t i = 0; i < 8; i++)
    {
      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] Run Task1 on Core: %d\r\n", millis(), xPortGetCoreID());
      xSemaphoreGive(mutexSerial);
      
      xSemaphoreTake(mutexSPI, portMAX_DELAY);
      CS_HC595_SET();
      SPI.transfer(1 << i);
      CS_HC595_RESET();
      xSemaphoreGive(mutexSPI);

      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void Task2(void* parameters)
{
  while (1)
  {
    max7219.Clean();
    max7219.DecodeOn();

    xSemaphoreTake(mutexSerial, portMAX_DELAY);
    Serial.printf("[%8lu] Run Task2 on Core: %d\r\n", millis(), xPortGetCoreID());
    xSemaphoreGive(mutexSerial);
    
    xSemaphoreTake(mutexSPI, portMAX_DELAY);
    max7219.PrintNtos(8, millis(), 8);
    xSemaphoreGive(mutexSPI);

    vTaskDelay(973 / portTICK_PERIOD_MS);
  }  
}

void Task3(void* parameters)
{
  pinMode(KEY_PIN, INPUT);

  while (1)
  {
    if(!digitalRead(KEY_PIN) && !keyFlag)
    {
      keyFlag = true;

      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] KEY is pressed!\r\n", millis());
      xSemaphoreGive(mutexSerial);      
    }

    if (digitalRead(KEY_PIN) && keyFlag)
    {
      keyFlag = false;

      xSemaphoreTake(mutexSerial, portMAX_DELAY);
      Serial.printf("[%8lu] KEY is unpressed!\r\n", millis());
      xSemaphoreGive(mutexSerial);

      if (task4Active)
      {
        task4Active = false;
        vTaskSuspend(Task2Handle);
      }
      else
      {
        task4Active = true;        
        vTaskResume(Task2Handle);
      }           
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  
}

void Task4(void* parameters)
{ 
  uint8_t brightness[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 
                           0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
  
  while (1)
  {
      for (size_t i = 0; i < sizeof(brightness) / sizeof(brightness[0]); i++)
      {
        xSemaphoreTake(mutexSPI, portMAX_DELAY);
        max7219.SetIntensivity(brightness[i]);
        xSemaphoreGive(mutexSPI);

        vTaskDelay(150 / portTICK_PERIOD_MS); 
      }            
  }  
}

void Task5(void* parameters)
{
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_BLUE, 0);

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