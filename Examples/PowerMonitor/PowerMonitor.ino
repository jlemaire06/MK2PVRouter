// PowerMonitor.ino

// Test the PowerMonitor class

#include "PowerMonitor.h" 
#include <elapsedMillis.h>          

byte tPin[N] = {3, 5, 4};
double tCal[N] = {1.01, 0.055, 0.055};
int tPha[NM1] = {312, 279}; // {256, 256}
PowerMonitor pm(125, 2, tPin, tCal, tPha); 

elapsedMillis tmr;    
double Vrms, Irms, Pr, Pa;       

void setup()
{  
  Serial.begin(9600);
  pm.Init();
}

void loop()
{
  // delay(30);
  pm.Process();
  if (tmr>2000)
  {
    tmr = 0;
#ifdef TESTING
    Serial.print("MaxSizeBuffer = "); Serial.print(pm.MaxSize());
    Serial.print(" NbSamples = "); Serial.print(pm.NbSamples());
    Serial.print(" MaxProcTime = "); Serial.print(pm.MaxProcTime()); Serial.println("us");
#else
    Vrms = pm.RmsVoltage();
    Irms = pm.RmsCurrent(0);
    Pr = pm.RealPower(0);
    Pa = Vrms*Irms;
    Serial.print("Vrms = "); Serial.print(Vrms); Serial.print("V");
    Serial.print(" Irms = "); Serial.print(Irms); Serial.print("A");
    Serial.print(" Pr = "); Serial.print(Pr); Serial.print("W");
    Serial.print(" Pa = "); Serial.print(Vrms*Irms); Serial.print("VA");
    Serial.print(" F = "); Serial.println(abs(Pr)/Pa);
#endif
  }
}
