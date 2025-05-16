#include <Arduino.h>
#include <STM32FreeRTOS.h>

// Pin definitions
#define LED_BLINK_PIN PA5
#define LED_TOGGLE_PIN PA6
#define BUTTON_PIN PA0
#define TRIG_PIN PA1
#define ECHO_PIN PA2

// Task handles (optional, for future use)
TaskHandle_t blinkTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t sensorTaskHandle;

// Shared flag or queue (optional for inter-task comm)
volatile bool buttonPressed = false;

// --- Task 1: Blink LED at 500ms rate ---
void BlinkLEDTask(void *pvParameters) {
  pinMode(LED_BLINK_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED_BLINK_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED_BLINK_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// --- Task 2: Button press toggles other LED and sends UART message ---
void ButtonTask(void *pvParameters) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_TOGGLE_PIN, OUTPUT);
  bool lastState = HIGH;
  
  for (;;) {
    bool currentState = digitalRead(BUTTON_PIN);
    
    if (lastState == HIGH && currentState == LOW) {
      // Debounce delay
      vTaskDelay(pdMS_TO_TICKS(50));
      if (digitalRead(BUTTON_PIN) == LOW) {
        // Toggle LED
        digitalWrite(LED_TOGGLE_PIN, !digitalRead(LED_TOGGLE_PIN));
        Serial.println("Button Pressed!");
      }
    }

    lastState = currentState;
    vTaskDelay(pdMS_TO_TICKS(10)); // Polling interval
  }
}

// --- Task 3: Read distance from ultrasonic sensor and send over UART ---
void SensorTask(void *pvParameters) {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  for (;;) {
    // Trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure pulse duration
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout after 30ms

    // Calculate distance (cm)
    float distance = duration * 0.0343 / 2.0;

    // Check if valid and print
    if (duration > 0) {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("Sensor timeout");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait 1 second
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial monitor
  Serial.println("System Ready");

  // Create FreeRTOS tasks
  xTaskCreate(BlinkLEDTask, "BlinkLED", 128, NULL, 1, &blinkTaskHandle);
  xTaskCreate(ButtonTask, "Button", 128, NULL, 1, &buttonTaskHandle);
  xTaskCreate(SensorTask, "Sensor", 256, NULL, 1, &sensorTaskHandle);

  // Start scheduler
  vTaskStartScheduler();
}

void loop() {
  // Empty because RTOS tasks handle everything
}