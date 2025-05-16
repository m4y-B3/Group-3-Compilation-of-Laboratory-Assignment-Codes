/* STM32CubeIDE translation of Arduino code for
 * SSD1306 OLED, BME280 sensor, and HC-SR04 Ultrasonic
 * Target: STM32F411CEU6
 * I2C Pins: SCL -> PB8, SDA -> PB9
 * Ultrasonic: TRIG -> PA0, ECHO -> PA1
 */

#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "ssd1306.h"
#include "fonts.h"
#include "bme280.h"
#include <stdio.h>
#include <string.h>

#define TRIG_PIN GPIO_PIN_0
#define TRIG_PORT GPIOA
#define ECHO_PIN GPIO_PIN_1
#define ECHO_PORT GPIOA

BME280_t bme;
char buffer[64];

uint32_t readUltrasonicDistance();

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init(); // Ensure I2C1 uses PB8 (SCL), PB9 (SDA)
  MX_USART2_UART_Init();

  // Initialize SSD1306 OLED
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Initializing BME280...", Font_7x10, White);
  ssd1306_UpdateScreen();

  // Initialize BME280 sensor (I2C addr 0x76 or 0x77)
  if (BME280_Init(&bme, &hi2c1, 0x76) != BME280_OK)
  {
    sprintf(buffer, "BME280 not found!\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString("BME280 not found!", Font_7x10, White);
    ssd1306_UpdateScreen();
    while (1);
  }

  ssd1306_SetCursor(0, 20);
  ssd1306_WriteString("Initialization Done!", Font_7x10, White);
  ssd1306_UpdateScreen();
  HAL_Delay(2000);

  while (1)
  {
    BME280_ReadTemperature(&bme);
    BME280_ReadHumidity(&bme);
    BME280_ReadPressure(&bme);

    float temp = bme.temperature;
    float hum = bme.humidity;
    float pres = bme.pressure / 100.0f;
    uint32_t distance_cm = readUltrasonicDistance();

    sprintf(buffer, "Temp: %.2f C\r\n", temp);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    sprintf(buffer, "Hum : %.2f %%\r\n", hum);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    sprintf(buffer, "Pres: %.2f hPa\r\n", pres);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    sprintf(buffer, "Dist: %lu cm\r\n", distance_cm);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("STM32 + OLED + BME280", Font_7x10, White);

    ssd1306_SetCursor(0, 16);
    sprintf(buffer, "Temp: %.2f C", temp);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(0, 26);
    sprintf(buffer, "Hum : %.2f %%", hum);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(0, 36);
    sprintf(buffer, "Pres: %.2f hPa", pres);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_SetCursor(0, 48);
    sprintf(buffer, "Dist: %lu cm", distance_cm);
    ssd1306_WriteString(buffer, Font_7x10, White);

    ssd1306_UpdateScreen();
    HAL_Delay(500);
  }
}

uint32_t readUltrasonicDistance()
{
  uint32_t startTick = 0, stopTick = 0;

  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
  HAL_Delay(0.01); // ~10us pulse
  HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

  while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET);
  startTick = HAL_GetTick();

  while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET);
  stopTick = HAL_GetTick();

  uint32_t duration = stopTick - startTick;
  return duration * 34 / 2; // cm, rough conversion
}
