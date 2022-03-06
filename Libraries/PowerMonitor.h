// PowerMonitor.h

#pragma once

#include <Arduino.h>
#include "RingBuffer.hpp"

#define M 100 // Buffer size <256
#define N 3   // Number of channels : V, C1, C2, etc.
#define NM1 2 // N-1 = number of current channels    
#define TESTING   

/***********************************************************************************
  Class PowerMonitor
************************************************************************************

  Rms voltage, rms current(s) and real power(s) estimations for a voltage and one or
  many current channels, the voltage channel being the first
  
  Inspired from :
    Robin Emley's Cal_bothDisplays_2.ino sketch  ( https://www.mk2pvrouter.co.uk )
    EmonLib library ( https://github.com/openenergymonitor/EmonLib )
    EmonLibCM library ( https://github.com/openenergymonitor/EmonLibCM )

  Constructor parameters :
 
    _period    = sampling period for the channels (in us and 105<.<8192)
    _nCycl     = number of cycles to construct the estimations (>=1)
    _tPin[N]   = Arduino pins array for the channels
    _tCal[N]   = Calculus coefficient array to convert a digitized value
    _tPha[NM1] = Phase shift coefficient array of the voltage 
    NB : 256 => no shift, <256 => right shift, >256 => left shift
*/

enum Polarities {POSITIVE, NEGATIVE}; // Alternance polarities

extern "C" void TIMER1_OVF_vect();

class PowerMonitor
{
  public:
    // Constructor
    PowerMonitor(int _period, int _nCycl, byte  _tPin[], double _tCal[], int _tPha[]);  
    
    void Init();              // Initialisation (to be called in the setup)
    void Process();           // Process calculus outside the ISR (to be called in the loop()
    double RmsVoltage();      // Vrms (V)
    double RmsCurrent(int i); // Irms (A) with i=0 => CT1, i=1 => CT2, etc.
    double RealPower(int i);  // Pr (W) with i=0 => CT1, i=1 => CT2, etc.
  
#ifdef TESTING
    byte MaxSize();           // Maximum size of the circular buffer, on entering Process()
    int NbSamples();          // Number of samples in estimations
    long MaxProcTime();       // Max time to treat a sample (us)
#endif

  private:
    unsigned int period;      // Sampling period for the channels
    int nCycl, iCycl;         // Number of cycles and index of the cycle 
    byte* tPin;               // Arduino pins array for the channels
    double* tCal;             // Calculus coefficients array to convert the digitized vales
    int* tPha;                // Phase shift coefficients array of the voltage 
 
    int iChan;                // Index of the channel in the digitalization
    RingBuffer<M, N> rbDig;   // Circular buffer for the digitized values
    int tDigISR[N];           // Digitized values array pushed in the ISR 
    int tDig[N];              // Digitized values array poped in the Process method 
    long V;                   // Voltage value (<<8 and centered)
    long tC[NM1];             // Current values  array (<<8 and centered) 
    Polarities pol;           // Polarity of the alternance
 
    long offV;                // Offset voltage for centering
    long tOffC[NM1];          // Offset currents array for centering
    long sumV;                // Sum of the voltage values 
    long tSumC[NM1];          // Sum of the current values array
    long sumVV;               // Sum of the squared voltage values (>>10)
    long savSumVV;            // Saved sum of the squared voltage values (>>10)    
    long tSumCC[NM1];         // Sum of the squared current values array (>>10)
    long tSavSumCC[NM1];      // Saved sum of the squared current values array (>>10)
    long tSumVC[NM1];         // Sum of the VC products array (>>10)
    long tSavSumVC[NM1];      // Saved sum of the VC products array (>>10)
    long preV;                // Previous voltage value (>>2)
    long phaV;                // Phase shifted voltage value (>>2)
    int nSum;                 // Number (saved) of values in the sums VV, CC and VC
    int nSavSum;              // Number (saved) of values in the sums VV, CC and VC
    void FuncISR();           // ISR function

#ifdef TESTING
    byte maxSize;             // Maximum size of the circular buffer, on entering Process()
    long maxProcTime;         // Maximum of processing time
#endif
    
  friend void TIMER1_OVF_vect(); // To use the private FuncISR() in the ISR
};
