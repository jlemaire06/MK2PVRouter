// Displayer.cpp

#include "Displayer.h"

/***********************************************************************************
 Statics
***********************************************************************************/

// Mapping from symbols to segments

byte symMap[23] =
{
  0b00111111, //  #0 "0"
  0b00000110, //  #1 "1"
  0b01011011, //  #2 "2"
  0b01001111, //  #3 "3"
  0b01100110, //  #4 "4"
  0b01101101, //  #5 "5"
  0b01111101, //  #6 "6"
  0b00000111, //  #7 "7"
  0b01111111, //  #8 "8"
  0b01101111, //  #9 "9"
  0b10111111, // #10 "0."
  0b10000110, // #11 "1."
  0b11011011, // #12 "2."
  0b11001111, // #13 "3."
  0b11100110, // #14 "4."
  0b11101101, // #15 "5."
  0b11111101, // #16 "6."
  0b10000111, // #17 "7."
  0b11111111, // #18 "8."
  0b11101111, // #19 "9."  
  0b00000000, // #20 " " 
  0b10000000, // #21 "." 
  0b01000000  // #22 "-"   
};

/***********************************************************************************
 Class Displayer
***********************************************************************************/

Displayer::Displayer(byte _tSegPin[], byte _tDigPin[])
{
  tSegPin = _tSegPin;
  tDigPin = _tDigPin;
  for (int j = 0; j < 8; j++) 
  {
    pinMode(tSegPin[j], OUTPUT);
    digitalWrite(tSegPin[j], LOW); // Segment OFF
  }
  for (int i = 0; i < 4; i++)
  {
    pinMode(tDigPin[i], OUTPUT);
    digitalWrite(tDigPin[i], HIGH); // Digit DISABLED
    tSym[i] = 20;
  }  
  curDig = 0;
  tmrDig = 0;
}

void Displayer::SetSym(byte sym, byte dig) 
{
  tSym[dig] = sym;
}

void Displayer::SetSym(double x)
{
  int y = round(x);
  SetSym(y); 
  if (x != y && tSym[0] != 21)
  {
    // Add sign when y null and x negative
    if (y == 0 && x < 0) tSym[2] = 22;
      
    tSym[3] += 10; // Add dot
    
    // Number n of decimals and left align
    int n = 0;
    while (tSym[0] == 20)
    {
      for (int i = 0; i < 3; i++) tSym[i] = tSym[i+1];
      n++;
    }
    
    // Decimals
    x = abs(x - y);
    for (int i = 0; i < n; i++) x *= 10;
    y = round(x);
    int m = 3;
    do
    {
      tSym[m--] = y % 10; 
      y = y / 10;
      n--;
    } while (n > 0);
  } 
}

void Displayer::SetSym(int x)
{
  if (x < -999 || x > 9999) for (int i = 0; i < 4; i++) tSym[i] = 21;
  else
  {
    int y = abs(x);
    int n = 3;
    do
    {
      tSym[n--] = y % 10; 
      y = y / 10;
    } while (y > 0);
    if (x < 0) tSym[n--] = 22;
    while (n >= 0) tSym[n--] = 20;
  }
}

void Displayer::Refresh()
{
  if (tmrDig > DIGIT_PERIOD)
  {
    tmrDig = 0;
    digitalWrite(tDigPin[curDig], HIGH); // Digit DISABLED
    curDig = ++curDig % 4;
    digitalWrite(tDigPin[curDig], LOW);  // Digit ENABLED 
    byte seg = symMap[tSym[curDig]];
    
    // Segments of the digit (faster than bitRead)
    byte b = 1;
    for (int j = 0; j < 8; j++)
    {
      digitalWrite(tSegPin[j], seg & b); 
      b = b << 1;
    }
  } 
}
