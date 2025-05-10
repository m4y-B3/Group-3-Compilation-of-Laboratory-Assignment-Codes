#define LED_PIN PC13   // On-board LED on Black Pill

char receivedChar = 0;
bool ledState = false;
unsigned long lastHelloTime = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);  // USB Serial (CDC)
  while (!Serial);       // Wait for the Serial Monitor to open
}

void loop() {
  // Send "Hello" every 1 second
  if (millis() - lastHelloTime >= 1000) {
    Serial.println("Hello from STM32!");
    lastHelloTime = millis();
  }

  // Check if data is available
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == 't') {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }
}