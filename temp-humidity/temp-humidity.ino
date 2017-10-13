#include <dht.h>

dht DHT;                  // dht object

#define DHT11_PIN 2       // Temp and humidity sensor on pin 2

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);     // Red LED on pin 7
  pinMode(6, OUTPUT);     // Blue LED on pin 6
  Serial.print("Booting up LED test...\n");
  bootup_led_test();
  Serial.print("Starting temperature and humidity logging...\n\n");
}

void loop() {
  // put your main code here, to run repeatedly:
 output_pin_value();
 delay(30000);
}

void output_pin_value() {
  int sensor_val = DHT.read11(DHT11_PIN);

  // Serial output some debug info
  Serial.print("Temperature: ");
  Serial.print(get_temperature());
  Serial.print("\n");
  Serial.print("Humidity: ");
  Serial.print(get_humidity());
  Serial.print("\n\n");
  
  // Based on thresholds show red or blue LED
  if(get_temperature() > 70.0) {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
  }
}

float get_temperature() {
  return (9.0 / 5.0) * DHT.temperature + 32;  // C to F conversion formula
}

float get_humidity() {
  return DHT.humidity;
}

void bootup_led_test() {
  for(int i = 0; i <= 5; i++) {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    delay(1000);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    delay(1000);
  }
}

