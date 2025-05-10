#define LED_BLINK_PIN PA0  // LED for blinking (Demo 1)
#define LED_FADE_PIN  PA1  // LED for fading (PWM - Demo 2)

unsigned long previousMillis = 0;
const long blinkInterval = 500;  // 500ms for blinking

int brightness = 0;
int fadeAmount = 5;

void setup() {
  pinMode(LED_BLINK_PIN, OUTPUT);
  pinMode(LED_FADE_PIN, OUTPUT);
}

void loop() {
  // === DEMO 1: Blinking LED every 500ms ===
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    digitalWrite(LED_BLINK_PIN, !digitalRead(LED_BLINK_PIN)); // Toggle blink LED
  }

  // === DEMO 2: Fading LED using PWM ===
  analogWrite(LED_FADE_PIN, brightness);  // PWM signal
  brightness += fadeAmount;
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;  // Reverse fade direction
  }
  delay(30);  // Adjust fading speed
}