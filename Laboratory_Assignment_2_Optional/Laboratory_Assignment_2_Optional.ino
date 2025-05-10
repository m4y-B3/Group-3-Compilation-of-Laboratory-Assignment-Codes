#define LED_PIN PB12  // Change this to your actual LED pin
char commandBuffer[50];
int bufferIndex = 0;
bool ledState = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Start with LED OFF
  ledState = false;

  Serial.begin(115200);
  Serial.println("System Ready. Send commands: led_on, led_off, status?");
}

void loop() {
  // Handle serial command input
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      commandBuffer[bufferIndex] = '\0';
      parseCommand(commandBuffer);
      bufferIndex = 0;
    } else if (bufferIndex < sizeof(commandBuffer) - 1) {
      commandBuffer[bufferIndex++] = ch;
    }
  }

  // Print sensor voltage every 10 seconds
  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 10000) {
    lastSend = millis();
    int sensorValue = analogRead(A0); // Use your sensor's analog pin
    float voltage = sensorValue * (3.3 / 4095.0);  // For STM32 ADC 12-bit
    Serial.print("Sensor voltage: ");
    Serial.print(voltage, 2);
    Serial.println(" V");
  }
}

void parseCommand(const char* cmd) {
  if (strcmp(cmd, "led_on") == 0) {
    digitalWrite(LED_PIN, HIGH);  // LED ON
    ledState = true;
    Serial.println("LED turned ON");
  } else if (strcmp(cmd, "led_off") == 0) {
    digitalWrite(LED_PIN, LOW);   // LED OFF
    ledState = false;
    Serial.println("LED turned OFF");
  } else if (strcmp(cmd, "status?") == 0) {
    Serial.print("LED is currently ");
    Serial.println(ledState ? "ON" : "OFF");
  } else {
    Serial.println("Unknown command.");
  }
}