// Displayer3.ino

// Test 3 of the displayer
// Time to refresh

#include <Displayer.h>
#include <elapsedMillis.h>

#define PERIOD 500

byte tSegPin[] = {2, 5, 12, 6, 7, 9, 8, 14};
byte tDigPin[] = {16, 10, 13, 11};
Displayer disp(tSegPin, tDigPin);
elapsedMillis tmr;

void setup()
{
  Serial.begin(115200);
  disp.SetSym(3.14);
}

void loop()
{
  unsigned long duration = micros();
  disp.Refresh();
  duration = micros() - duration;
  if (tmr > PERIOD)
  {
    tmr = 0;
    Serial.print(duration); Serial.println("us ");
  }
}
