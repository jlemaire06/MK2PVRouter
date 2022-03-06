// PowerMonitor.cpp

#include "PowerMonitor.h"

/***********************************************************************************
ISR 
***********************************************************************************/

PowerMonitor* pPowerMonitor; // As static

ISR(TIMER1_OVF_vect)
{
  pPowerMonitor->FuncISR();
}

/***********************************************************************************
 Class PowerMonitor
***********************************************************************************/

PowerMonitor::PowerMonitor(int _period, int _nCycl, byte  _tPin[], double _tCal[], int _tPha[])
{
  pPowerMonitor = this;
  period = _period;
  nCycl = _nCycl;
  tPin = _tPin;
  tCal = _tCal;
  tPha = _tPha;
}

void PowerMonitor::Init()
{
  // Set ADC parameters (prescaler, voltage reference), enable and start it with the volatge source
  ADCSRA = (1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); // Prescaler = 128 (104us/conversion and max accuracy)
  ADCSRA |= (1<<ADEN);                       // Enable ADC
  ADMUX = 0x40;                              // Voltage reference = AVcc with external capacitor at AREF pin 
  ADMUX |= tPin[0];                          // Set ADPS to the voltage pin
  ADCSRA |= (1<<ADSC);                       // Start the conversion
  delay(1);                                  // Wait to perform this conversion
  
  // Set Timer1 interrupts 
  noInterrupts();                            // Disable all interrupts
  TCCR1A = 0;                                // Normal port operation (no PWM outputs)
  TCCR1B = (1<<WGM13);                       // Phase and frequency correct PWM mode with TOP in ICR1
  TCCR1B |= (1<<CS10);                       // No prescaling
  ICR1 = 8*period;                           // TOP
  TIMSK1 = (1<<TOIE1);                       // Enable timer OVF interrupt (TOV1 flag set on at BOTTOM)
  interrupts();                              // Enable all interrupts
  
  // Init values for members
  iCycl = 0;
  iChan = 0;
  nSum = 0;
  nSavSum = 0;
  offV = 510L*256;
  sumV = 0;
  sumVV = 0;
  for (int i=0; i<NM1; i++)
  {
    tOffC[i] = 510L*256;
    tSumC[i] = 0;
    tSumCC[i] = 0;
    tSumVC[i] = 0;
  }
  preV = 220L*64;
  
#ifdef TESTING
    maxSize = 0;
    maxProcTime = 0;
#endif
}

void PowerMonitor::Process()
{
  
#ifdef TESTING
    maxSize = max(maxSize, rbDig.Size());
#endif

  while (rbDig.Size() > 0)
  {

#ifdef TESTING
    long startTime = micros();
#endif

    // Pop the digitized values
    cli();
    rbDig.Pop(tDig);
    sei();

    // Voltage and current values (x256 and centering)
    V = (((long)tDig[0])<<8)- offV;
    for (int i=0; i<NM1; i++) tC[i] = (((long)tDig[i+1])<<8)- tOffC[i];

    // New half alternance test
    if ((V >= 0) &&  (pol == NEGATIVE)) // Start POSITIVE (new cycle)
    {
      // Polarity new value
      pol = POSITIVE;
      
      iCycl++;
      if (iCycl == nCycl)
      {
        // Save the sums of squared values and VC products, then reinit them
        savSumVV = sumVV;
        sumVV = 0;
        for (int i=0; i<NM1; i++) 
        {
          tSavSumCC[i] = tSumCC[i];
          tSavSumVC[i] = tSumVC[i];
          tSumCC[i] = 0;
          tSumVC[i] = 0;
        } 
        nSavSum = nSum;
        nSum = 0;
        
        // Reinit iCycl
        iCycl = 0;
      }
    }
    else if ((V < 0) &&  (pol == POSITIVE)) // Start NEGATIVE
    {
      // Polarity new value
      pol = NEGATIVE;

      // Offsets update using low pass filters 
      offV += (sumV>>12);
      sumV = 0;
      for (int i=0; i<NM1; i++) 
      { 
        tOffC[i] += (tSumC[i]>>12);
        tSumC[i] = 0;
      }
    }
    
    // Sums incrementation
    sumV += V;
    V = (V>>2);
    sumVV += ((V*V)>>6);
    for (int i=0; i<NM1; i++) 
    { 
      long C = tC[i];
      tSumC[i] += C;
      C = (C>>2);
      tSumCC[i] += ((C*C)>>6);
      phaV = preV + (((V - preV)*tPha[i])>>8);
      tSumVC[i] += ((phaV*C)>>6);
    }
    preV = V;
    nSum++;

#ifdef TESTING
    maxProcTime = max(maxProcTime, micros()-startTime);
#endif

  }
} 

double PowerMonitor::RmsVoltage()
{
  return tCal[0]*sqrt(((double)savSumVV)/(64.*nSavSum));
}

double PowerMonitor::RmsCurrent(int i)
{
  return tCal[i+1]*sqrt(((double)tSavSumCC[i])/(64.*nSavSum));
}

double PowerMonitor::RealPower(int i)
{
  return tCal[0]*tCal[i+1]*(((double)tSavSumVC[i])/(64.*nSavSum));
}

#ifdef TESTING
byte PowerMonitor::MaxSize()
{
  byte savMaxSize = maxSize;
  maxSize = 0;
  return savMaxSize;
}

int PowerMonitor::NbSamples()
{
  return nSavSum;
}
           
long PowerMonitor::MaxProcTime()
{
  long savMaxProcTime = maxProcTime;
  maxProcTime = 0;
  return savMaxProcTime;
}
#endif

void PowerMonitor::FuncISR()
{
  // Digitized value
  tDigISR[iChan++] = ADC; 
  
  // Push the digitized values in the buffer
  if(iChan == N) 
  {
    iChan = 0;
    rbDig.Push(tDigISR);
  }
    
  // Start the next conversion 
  ADMUX = 0x40 | tPin[iChan];
  ADCSRA |= (1<<ADSC); 
}  
