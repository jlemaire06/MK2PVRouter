// Displayer2.ino

// Test 2 of the displayer
// Display different integer or real numbers

#include <Displayer.h>
#include <elapsedMillis.h>

#define DISPLAY_PERIOD 1000 // For displaying a number (ms)
#define NO_NUMBERS 10

byte tSegPin[] = {2, 5, 12, 6, 7, 9, 8, 14};
byte tDigPin[] = {16, 10, 13, 11};
Displayer disp(tSegPin, tDigPin);
elapsedMillis tmr;
double tNumber[NO_NUMBERS] = {213, -213, -1234, 9999, 10240, 3.1416, -3.1416, 0.00123, 5E+3, -5E+3};
byte id;

void setup()
{
  id = 0;
  disp.SetSym(tNumber[0]);
}

void loop()
{
  disp.Refresh();
  if (tmr > DISPLAY_PERIOD)
  {
    tmr = 0;
    id = ++id % NO_NUMBERS;
    disp.SetSym(tNumber[id]);
  }
}
