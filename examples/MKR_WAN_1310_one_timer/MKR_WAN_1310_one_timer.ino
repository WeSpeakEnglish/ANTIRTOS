// SAMD21D ARDUINO MKR WAN 1310
//  utilize only one timer (TIMER_TC3), on different intervals push task functions and parameters
//  into queue

// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the
// system. Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the
// system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// Select only one to be true for SAMD21. Must must be placed at the beginning before #include
// "SAMDTimerInterrupt.h"
#define USING_TIMER_TC3 true  // Only TC3 can be used for SAMD51
#define USING_TIMER_TC4 false // Not to use with Servo library
#define USING_TIMER_TC5 false
#define USING_TIMER_TCC false
#define USING_TIMER_TCC1 false
#define USING_TIMER_TCC2 false // Don't use this, can crash on some boards

#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

#include <antirtos.h>

// TC3, TC4, TC5 max permissible TIMER_INTERVAL_MS is 1398.101 ms, larger will overflow, therefore
// not permitted Use TCC, TCC1, TCC2 for longer TIMER_INTERVAL_MS
#define HW_TIMER_INTERVAL_MS 50L

#define TIMER_INTERVAL_1S 1000L
#define TIMER_INTERVAL_2S 2000L

// Init selected SAMD timer
SAMDTimer ITimer3(TIMER_TC3);

// Init SAMD_ISR_Timer
// Each SAMD_ISR_Timer can service 16 different ISR-based timers
SAMD_ISR_Timer ISR_Timer;

void TimerHandler(void)
{
  ISR_Timer.run();
}

class testClass
{ // it is not used here, just like example how you may pass complex argument to your functions in
  // queue
public:
  int array[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float argument = 0.0;
};

Antirtos::tQ<10, uint32_t> F1();
Antirtos::tQ<10, testClass> F2(); // it is not used here, just like example how you may pass complex
                                  // argument to your functions in queue

void printMessage1(
    uint32_t milliseconds) // ISR function excutes when push button at pinD2 is pressed
{
  Serial.print("TIMER_INTERVAL_1S elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void printMessage2(
    uint32_t milliseconds) // ISR function excutes when push button at pinD3 is pressed
{
  Serial.print("TIMER_INTERVAL_2S elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void doingSomething1()
{
  F1.push(printMessage1,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void doingSomething2()
{
  F1.push(printMessage2,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

  // Interval in millisecs
  if (ITimer3.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = "));
    Serial.println(millis());
  }
  else
  {
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
  }

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_Timer
  ISR_Timer.setInterval(TIMER_INTERVAL_1S, doingSomething1);
  ISR_Timer.setInterval(TIMER_INTERVAL_2S, doingSomething2);
}

void loop()
{
  // put your main code here, to run repeatedly:
  F1.pull();
}
