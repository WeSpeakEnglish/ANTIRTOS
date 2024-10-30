#include <antirtos.h>

typedef struct pinOut
{ // structure (index - pin number, logic - 1/0 = ON/OFF)
  int index;
  int logic;
  int period;
} pinout;

antirtos::TaskQd<10, pinout> Q1(); // maximum 10 function pointers with parameters in queue

void writePin(pinout cmd)
{ // write a pin logic

  digitalWrite(cmd.index, cmd.logic);
  if (cmd.logic) // inverting logic
  {
    cmd.logic = LOW;
  }
  else
  {
    cmd.logic = HIGH;
  }

  Q1.push_delayed(writePin, cmd, cmd.period); // add this function into a queue again
}

void revokeF(pinout dummy)
{ // compatibility
  Q1.revoke(writePin);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // timet setup
  TCCR1A = 0x00; // Normal Mode
  TCCR1B = 0x00; // TC1 is OFF
  TCNT1 = 0;
  OCR1A = 6250;           // 0.1s delay; prescaler 256
  bitSet(TIMSK1, OCIE1A); // local intterupt is active
  TCCR1B |= bit(CS12);    // Start TC1 with prescale 256

  Q1.push_delayed(writePin, {5, HIGH, 5}, 5);      // period 1 sec.
  Q1.push_delayed(writePin, {6, HIGH, 10}, 10);    // period 2 sec.
  Q1.push_delayed(writePin, {7, HIGH, 20}, 20);    // period 4 sec.
  Q1.push_delayed(writePin, {8, HIGH, 40}, 40);    // period 8 sec.
  Q1.push_delayed(writePin, {9, HIGH, 80}, 80);    // period 16 sec.
  Q1.push_delayed(writePin, {10, HIGH, 160}, 160); // period 32 sec.
  Q1.push_delayed(writePin, {11, HIGH, 320}, 320); // period 64 sec.
  Q1.push_delayed(writePin, {12, HIGH, 640}, 640); // period 128 sec.
  Q1.push_delayed(revokeF, {0, 0, 0}, 3000);       // stop after 300 sec.
}

void loop()
{
  // put your main code here, to run repeatedly:
  Q1.pull(); // pull from the queue
}

ISR(TIMER1_COMPA_vect) // timer interrupt ticks one per 0.1 sec
{
  TCNT1 = 0;
  OCR1A = 6250;
  Q1.tick(); // execute tick method for make delayed functionality works
}
