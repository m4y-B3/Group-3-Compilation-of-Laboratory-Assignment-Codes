// LED Pins
const int pwmLed1 = PB6;  // TIM4_CH1
const int pwmLed2 = PB7;  // TIM4_CH2

// ADC Pin
const int potPin = PA0;  // ADC channel

int brightness = 0;
unsigned long previousMillis = 0;

void setup() {
  pinMode(pwmLed1, OUTPUT);
  pinMode(pwmLed2, OUTPUT);

  analogWriteResolution(8);  // 8-bit PWM: 0-255
}

void loop() {
  // Read potentiometer value (0-1023)
  int potValue = analogRead(potPin);

  // Map potentiometer value to a delay between brightness changes
  int delayTime = map(potValue, 0, 1023, 5, 50); // Shorter delay = faster fade

  // Fade LED1 up and down using PWM
  for (brightness = 0; brightness <= 255; brightness++) {
    analogWrite(pwmLed1, brightness);
    delay(delayTime);
  }

  for (brightness = 255; brightness >= 0; brightness--) {
    analogWrite(pwmLed1, brightness);
    delay(delayTime);
  }

  // Set LED2 to a fixed duty cycle (e.g., 30%)
  analogWrite(pwmLed2, 76);  // 30% of 255 = ~76
}