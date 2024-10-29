
// The example demonstrate usage of several different times on SAMD21 platforms like MKR WAN.
// In each timer interrupt we push our task function and a parameter to another queue.
// All the tasks easily handled in loop via pulling them from the queues.
// The interrupts are kept short, revealing the unblocking of the rest of the programme.

// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the
// system. Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the
// system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// Must must be placed at the beginning before #include "SAMDTimerInterrupt.h"
#define USING_TIMER_TC3 true
#define USING_TIMER_TC4 true
#define USING_TIMER_TC5 true
#define USING_TIMER_TCC true
#define USING_TIMER_TCC1 true
#define USING_TIMER_TCC2 false // Don't use this, can crash on some boards

#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

#include <antirtos.h>

// TC3, TC4, TC5 max permissible TIMER_INTERVAL_MS is 1398 ms, larger will overflow, therefore not
// permitted Use TCC, TCC1, TCC2 for longer TIMER_INTERVAL_MS
uint32_t TIMER_INTERVAL_MS[MAX_TIMER] = {200, 500, 1000, 2000, 5000};

SAMDTimer ITimer0(TIMER_TC3);
SAMDTimer ITimer1(TIMER_TC4);
SAMDTimer ITimer2(TIMER_TC5);
SAMDTimer ITimer3(TIMER_TCC);
SAMDTimer ITimer4(TIMER_TCC1);

tQ<10, uint32_t> F1();
tQ<10, uint32_t> F2();
tQ<10, uint32_t> F3();
tQ<10, uint32_t> F4();
tQ<10, uint32_t> F5();

void TimerHandler_TIMER_TC3()
{
  F1.push(printMessage1,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void TimerHandler_TIMER_TC4()
{
  F2.push(printMessage2,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void TimerHandler_TIMER_TC5()
{
  F3.push(printMessage3,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void TimerHandler_TIMER_TCC()
{
  F4.push(printMessage4,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void TimerHandler_TIMER_TCC1()
{
  F5.push(printMessage5,
          millis()); // add your task into queue, keep your interrupt as fast as possible
}

void printMessage1(
    uint32_t milliseconds) // ISR function excutes when push button at pinD2 is pressed
{
  Serial.print("TIMER_TC3 elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void printMessage2(
    uint32_t milliseconds) // ISR function excutes when push button at pinD3 is pressed
{
  Serial.print("TIMER_TC4 elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void printMessage3(
    uint32_t milliseconds) // ISR function excutes when push button at pinD3 is pressed
{
  Serial.print("TIMER_TC5 elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void printMessage4(
    uint32_t milliseconds) // ISR function excutes when push button at pinD2 is pressed
{
  Serial.print("TIMER_TCC elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void printMessage5(
    uint32_t milliseconds) // ISR function excutes when push button at pinD3 is pressed
{
  Serial.print("TIMER_TCC1 elapsed");
  Serial.print(" at ");
  Serial.println(milliseconds);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

#if USING_TIMER_TC3
  if (ITimer0.attachInterruptInterval_MS(TIMER_INTERVAL_MS[TIMER_TC3], TimerHandler_TIMER_TC3))
  {
    Serial.println("Starting  TIMER_TC3 OK, millis() = " + String(millis()));
  }
  else
  {
    Serial.println("Can't set TIMER_TC3. Select another freq. or timer");
  }
#endif

#if USING_TIMER_TC4
  if (ITimer1.attachInterruptInterval_MS(TIMER_INTERVAL_MS[TIMER_TC4], TimerHandler_TIMER_TC4))
  {
    Serial.println("Starting  TIMER_TC4 OK, millis() = " + String(millis()));
  }
  else
  {
    Serial.println("Can't set TIMER_TC4. Select another freq. or timer");
  }
#endif

#if USING_TIMER_TC5
  if (ITimer2.attachInterruptInterval_MS(TIMER_INTERVAL_MS[TIMER_TC5], TimerHandler_TIMER_TC5))
  {
    Serial.println("Starting  TIMER_TC5 OK, millis() = " + String(millis()));
  }
  else
  {
    Serial.println("Can't set TIMER_TC5. Select another freq. or timer");
  }
#endif

#if USING_TIMER_TCC
  if (ITimer3.attachInterruptInterval_MS(TIMER_INTERVAL_MS[TIMER_TCC], TimerHandler_TIMER_TCC))
  {
    Serial.println("Starting  TIMER_TCC OK, millis() = " + String(millis()));
  }
  else
  {
    Serial.println("Can't set TIMER_TCC. Select another freq. or timer");
  }
#endif

#if USING_TIMER_TCC1
  if (ITimer4.attachInterruptInterval_MS(TIMER_INTERVAL_MS[TIMER_TCC1], TimerHandler_TIMER_TCC1))
  {
    Serial.println("Starting  TIMER_TCC1 OK, millis() = " + String(millis()));
  }
  else
  {
    Serial.println("Can't set TIMER_TCC1. Select another freq. or timer");
  }
#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
  F1.pull();
  F2.pull();
  F3.pull();
  F4.pull();
  F5.pull();
}
