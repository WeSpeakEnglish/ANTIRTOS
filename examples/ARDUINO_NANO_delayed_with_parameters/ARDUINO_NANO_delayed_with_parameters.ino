#include <antirtos.h>

typedef struct pinOut
{ // structure (index - pin number, logic - 1/0 = ON/OFF)
  int index;
  bool logic;
} pinout;

antirtos::TaskQd<8, pinout> Q1(); // maximum 8 function pointers with parameters in queue

void writePin(pinout cmd)
{ // write a pin true =- ON

  digitalWrite(cmd.index, cmd.logic);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  TCCR1A = 0x00; // Normal Mode
  TCCR1B = 0x00; // TC1 is OFF
  TCNT1 = 0;
  OCR1A = 6250;           // 0.1s delay; prescaler 256
  bitSet(TIMSK1, OCIE1A); // local intterupt is active
  TCCR1B |= bit(CS12);    // Start TC1 with prescale 256

  Q1.push_delayed(writePin, {12, true}, 20);  // yellow led ON after 2 sec. (0.1*20 = 2 seconds)
  Q1.push_delayed(writePin, {12, false}, 30); // yellow led OFF after 3 sec.
  Q1.push_delayed(writePin, {13, true}, 50);  // red led ON after 5 sec.
  Q1.push_delayed(writePin, {13, false}, 80); // red led OFF after 8 sec.
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
