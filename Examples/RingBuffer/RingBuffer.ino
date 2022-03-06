// RingBuffer.ino

// Test the RingBuffer class

#include "RingBuffer.hpp"           

RingBuffer<3,2> rb;   // [SE[.,.],  [.,.],  [.,.]] size=0        

void setup()
{  
  Serial.begin(9600);
  int s[2] = {0, 0};
  int t[2];
  rb.Push(s);         // [S [0,0], E[.,.],  [.,.]] size=1 
  s[0]++; rb.Push(s); // [S [0,0],  [1,0], E[.,.]] size=2 
  s[0]++; rb.Push(s); // [SE[0,0],  [1,0],  [2,0]] size=3 
  rb.Pop(t);          // [ E[.,.],S [1,0],  [2,0]] size=2
  Serial.print(rb.Size()); Serial.print("   "); Serial.print(t[0]); Serial.print(" "); Serial.println(t[1]);
  s[0]++; rb.Push(s); // [  [3,0],SE[1,0],  [2,0]] size=3
  rb.Pop(t);          // [  [3,0], E[.,.],S [2,0]] size=2
  rb.Pop(t);          // [S [3,0], E[.,.],  [.,.]] size=1
  rb.Pop(t);          // [SE[.,.],  [.,.],  [.,.]] size=0
  Serial.print(rb.Size()); Serial.print("   "); Serial.print(t[0]); Serial.print(" "); Serial.println(t[1]);
}

void loop() {}
