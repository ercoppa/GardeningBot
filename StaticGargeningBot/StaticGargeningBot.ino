#include <LowPower.h>

#define LED           LED_BUILTIN
#define PUMP          8   
#define WATERING_TIME 40L  // secs, do not remove the L suffix! See note below.

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); 
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW); 
}

void loop() {

  delay(1000);
  
  digitalWrite(PUMP, HIGH);
  // for some reason, the compiler computes this as short int, 
  // then if have something larger then 32K it will overflow!
  // Then the negative number if passed to delay, which takes 
  // an 4 byte integer, treating the negative number as a very large int!
  // Two solutions: (a) split delay into different delays (e.g., 30 + 10);
  //                (b) force the number to be treated as 4 byte integer
  //                    using the L suffix.
  delay(WATERING_TIME * 1000); 
  digitalWrite(PUMP, LOW);    

  for (int i = 0; i < 675; i++) { // 675 * (1 + 8 * 8) = ~12,2 hours
    // power on a led every 1 min to notify that it is still running
    digitalWrite(LED, HIGH);
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    digitalWrite(LED, LOW); 
    for (int k = 0; k < 8; k++) // sleep 64 secs
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
