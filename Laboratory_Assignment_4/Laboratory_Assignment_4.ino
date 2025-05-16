// ADC and UART example for STM32 (e.g., STM32F411CEU6)

const int analogPin = PA0;         // ADC input pin
const float Vref = 3.3;           // Reference voltage (adjust if needed)
const int ADCResolution = 4095;   // 12-bit ADC (2^12 - 1)

void setup() {
 Serial.begin(115200);           // Start UART communication
  analogReadResolution(12);       // Set ADC resolution to 12 bits (STM32 specific)
  pinMode(analogPin, INPUT);
}

void loop() {
   // 1. Read the raw ADC value
  int rawADC = analogRead(analogPin);

  // 2. Convert to voltage
  float voltage = (rawADC * Vref) / ADCResolution;

  // 3. Convert to percentage
  float percentage = (voltage / Vref) * 100.0;

  // 4. Send to Serial
  Serial.print("ADC: ");
  Serial.print(rawADC);
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);  // 3 decimal places
  Serial.print(" V\t");
  Serial.print("Percent: ");
  Serial.print(percentage, 1);
  Serial.println(" %");

  delay(200);  // 200 ms delay
}

