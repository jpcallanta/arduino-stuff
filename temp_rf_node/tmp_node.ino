#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include "RF24.h"

#define DHT11_PIN 3

// Globals
RF24 radio(7,8);
dht DHT;

void setup() {
  Serial.begin(115200);
  setup_radio();
  setup_dht();
  
}

void loop() {
    send_data();
    delay(60000);
}

void setup_radio() {
  byte addresses[][6] = {"1Node","2Node"};
  
  Serial.println("Setting up RF sender...");
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
}

void setup_dht() {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
}

int get_temp() {
  int sensor_val = DHT.read11(DHT11_PIN);

  return (9.0 / 5.0) * DHT.temperature + 32;
}

int get_humidity() {
  return DHT.humidity;
}

void send_data() {
  char tmp[] = "";
  char buff[] = "";

  // Payload formation is [location,temperature,humidity,voltage]
  sprintf(buff, "sun,%i,%i,%i", get_temp(), get_humidity(), read_volts());
  
  radio.stopListening();
  
  if(!radio.write(&buff, strlen(buff))) {
    sprintf(tmp, "Failed to send %s", buff);
    Serial.println(tmp);
  } else {
    sprintf(tmp, "Sent data [%s]", buff);
    Serial.println(tmp);
  }
}

// Modified a bit from https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
int read_volts() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result * 0.001; // Vcc in volts
}
