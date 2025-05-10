#include <Arduino.h>
#include <STM32FreeRTOS.h> // included by default in STM32Duino core

#define LED_BLINK PA5
#define LED_TOGGLE PA6
#define BUTTON_PIN PA0

TaskHandle_t ButtonTaskHandle;

// Interrupt flag
volatile bool buttonInterrupt = false;

// --- LED Task: blinks every 500ms ---
void LEDBlinkTask(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    digitalWrite(LED_BLINK, !digitalRead(LED_BLINK));
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// --- Button Task: waits for notification ---
void ButtonTask(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    // Wait until ISR notifies us
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Toggle LED and print message
    digitalWrite(LED_TOGGLE, !digitalRead(LED_TOGGLE));
    Serial.println("Button Pressed!");

    // Optional small delay (debounce-like pause)
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// --- Button Interrupt Handler ---
volatile uint32_t lastInterruptTime = 0;

void buttonISR() {
  uint32_t currentTime = millis();
  if (currentTime - lastInterruptTime > 200) { // 200ms debounce
    lastInterruptTime = currentTime;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(ButtonTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void setup() {
  pinMode(LED_BLINK, OUTPUT);
  pinMode(LED_TOGGLE, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  while (!Serial); // Wait for Serial to connect

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Create Tasks
  xTaskCreate(LEDBlinkTask, "LED Blink", 128, NULL, 1, NULL);
  xTaskCreate(ButtonTask, "Button Task", 128, NULL, 1, &ButtonTaskHandle);

  // Start FreeRTOS
  vTaskStartScheduler();
}

void loop() {
  // not used with FreeRTOS
}
