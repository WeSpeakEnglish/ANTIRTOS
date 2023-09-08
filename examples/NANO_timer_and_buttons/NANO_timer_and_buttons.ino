#include <antirtos.h>
#include<LiquidCrystal.h>                     // Including lcd display library

LiquidCrystal lcd (7,8,9,10,11,12);          //  LCD pins RS,E,D4,D5,D6,D7

fQ F1(8);
fQ F2(8);
fQP<int32_t> F3(10);

void button1pressedF()           //ISR function excutes when push button at pinD2 is pressed
{                    
              
   lcd.setCursor(0,1);                                          
   lcd.print("Button 1 pressed");

}


void button2pressedF()           //ISR function excutes when push button at pinD3 is pressed    
{  
        
   lcd.setCursor(0,1);                                          
   lcd.print("Button 2 pressed");
   
}


void timerElapsedF(int32_t param)
{
   lcd.clear(); 
   lcd.setCursor(0,0);                                          
   lcd.print("TIMER: ");
   lcd.print(param);   
   if(param % 2) digitalWrite(13,LOW);     //Turns LED ON or OFF depending upon output value
   else digitalWrite(13,HIGH);     //Turns LED ON or OFF 
}

void button1pressed(){
  F1.push(button1pressedF); // add your task into queue, keep your interrupt as fast as possible 
}

void button2pressed()
{
  F2.push(button2pressedF); // add your task into queue, keep your interrupt as fast as possible 
}

ISR(TIMER1_COMPA_vect)
{
  static uint32_t counter = 0; 
  F3.push(timerElapsedF, counter++);  // add your task into queue, keep your interrupt as fast as possible
}

void setup()                                                      
{

  lcd.begin(16,2);                              //  setting LCD as 16x2 type
  lcd.setCursor(0,0);
  lcd.clear();                                                    
  pinMode(13,OUTPUT);                                           
                 
  attachInterrupt(digitalPinToInterrupt(2),button1pressed,RISING);  //  function for creating external interrupts at pin2 on Rising (LOW to HIGH)
  attachInterrupt(digitalPinToInterrupt(3),button2pressed,RISING);  //  function for creating external interrupts at pin3 on Rising (LOW to HIGH)   

  //timer setup

  cli(); // disable interrupts during setup
 
  // Configure Timer 1 interrupt
  // F_clock = 16e6 Hz, prescaler = 64, Fs = 10 Hz
  TCCR1A = 0;
  TCCR1B = 1<<WGM12 | 0<<CS12 | 1<<CS11 | 1<<CS10;
  TCNT1 = 0;          // reset Timer 1 counter
  // OCR1A = ((F_clock / prescaler) / Fs) - 1 = 2499
  OCR1A = 2499;       // Set sampling frequency Fs = 10 Hz
  TIMSK1 = 1<<OCIE1A; // Enable Timer 1 interrupt
 
  sei(); // re-enable interrupts
}

void loop() {
  // put your main code here, to run repeatedly:
  F1.pull();
  F2.pull();
  F3.pull();
}
