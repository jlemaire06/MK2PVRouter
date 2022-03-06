// Displayer.h

#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#define DIGIT_PERIOD 5 // In ms

/***********************************************************************************
 Class Displayer
************************************************************************************

Simple class to manage a 8-segment 4-digit with a common cathode displayer for numbers

Inspired from :
- Robin Emley codes (https://mk2pvrouter.co.uk/downloads.html)
- Dean Reading SevSeg library (https://github.com/DeanIsMe/SevSeg)

 500001   500001   500001   500001
 5    1   5    1   5    1   5    1
 5    1   5    1   5    1   5    1
 466662   466662   466662   466662
 4    2   4    2   4    2   4    2
 4    2   4    2   4    2   4    2 
 433332 7 433332 7 433332 7 433332 7

NB 
 - 23 possible symbols only for each digit : "0", "1", ..., "9", "0.", "1.", ..., "9.", " ", ".", "-"
 - Acceptable numbers : -999 <= x <= 9999 ("...." displayed if not)
 - Driven by 12 digital pins : 8 => segments and 4 => digits
 - A segment is ON/OFF when its pin is HIGH/LOW
 - A digit is ENABLED/DISABLED when its pin is LOW/HIGH
 - Each digit is ENABLED during DIGIT_PERIOD ms
*/

class Displayer
{
public:
  Displayer(byte _tSegPin[], byte _tDigPin[]); // Constructor
  void SetSym(byte sym, byte dig);             // Set a symbol for a digit and " " for the other digits
  void SetSym(double x);                       // Set symbols for a double
  void SetSym(int x);                          // Set symbols for an integer
  void Refresh();                              // To call in the loop() function
 
private:
  byte* tSegPin;         // Segment pins (size = 8)
  byte* tDigPin;         // Digit pins (size = 4)
  byte tSym[4];          // Symbols (#) to display
  byte curDig;           // Current digit
  elapsedMillis tmrDig;  // Timer for displaying a digit
};
