// Displayer1.ino

// Test 1 of the displayer
// Successively display "0", "1", ..., "9" on each digit

#include <Displayer.h>
#include <elapsedMillis.h>

#define DISPLAY_PERIOD 500 // For displaying a symbol (ms)

byte tSegPin[] = {2, 5, 12, 6, 7, 9, 8, 14};
byte tDigPin[] = {16, 10, 13, 11};
Displayer disp(tSegPin, tDigPin);
byte dig, sym;
elapsedMillis tmr;

void setup()
{
  for (int i = 0; i < 4; i++) disp.SetSym(20, i); // Display none
  dig = 0;
  sym = 0;
  disp.SetSym(sym, dig);
}

void loop()
{
  disp.Refresh();
  if (tmr > DISPLAY_PERIOD)
  {
    tmr = 0;
    sym++;
    if (sym > 9) 
    {
      sym = 0;
      disp.SetSym(20, dig);
      dig = ++dig % 4;
    } 
    disp.SetSym(sym, dig); 
  }
}
