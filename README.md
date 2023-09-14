# ANTIRTOS 
### *No any RTOS needed, you will see...*

Imagine you are using little MCU and want to save space or do not have enough resources.
### Or maybe do not want to use RTOS or not possible, because…
1.	Do not want to learn RTOS operations and critical section stuff.
2.	Difficult to debug.
3.	Save resources and manage your code, not RTOS.
4.	No RTOS is ported to your platform, for example, in the case of very small and cheap microcontrollers.

### Here is the solution, using function pointers queues easily and make:
1.	Interrupts kept fast and controllable, no blocking anymore.
2.	Easily to debug and understand. 
3.	You want to have easy way to get multitasking.
4.	No dummy waiting/blockings. Wait by doing!

Allow your MCU to perform many tasks, while additionally using a very small amount of microcontroller memory. Manage function pointers queues in easy way, just through you task there, and pull them from a main loop. This approach allows you to keep interrupts fast and at the same time control multitasking in a simple and transparent way. Pass arguments to you function, they will be saved in separate queue and handled.

### Let’s explain a bit on examples.

0. To start using the library - include the header:
```cpp
#include <antirtos.h>
```

1.	Create a queues in an easy way
```cpp
fQ F1(8); // first queue is 8 elements(function pointers) long
fQ F2(16); // second queue is 16 elements(function pointers) long
fQ F3(10); // third queue is 10 elements(function pointers) long
fQP<int32_t> F4(10); // third queue is 10 elements(function pointers)
					 // functions are receiving int32_t argument
```


2.	Wherever you want, just push the pointers (and arguments if they needed)
```cpp
void button1Interrupt(){
	F1.push(dealAssociatedButton1); // void dealAssociatedButton1() – is your task for this button
}
  
void button2Interrupt(){
	F2.push(dealAssociatedButton2);         // void dealAssociatedButton2() – is your task for this button
	F2.push(secondDealAssociatedButton2);   // void secondDealAssociatedButton2() – 
                                                // is your second task for this button
}
void timer1ElapsedInterrupt(){
	F3.push(dealAssociatedTimer1);  // void dealAssociatedTimer1() – 
                                        // is your task for this timer1 event 
}
void timer2ElapsedInterrupt(){
	F4.push(dealAssociatedTimer2, yourIntArg);  // void dealAssociatedTimer2() – 
                                                // is your task for this timer2 event and variable yourIntArg
                                                //  passed like the argument 
}
``` 
3.	In the main loop:
```cpp
void loop() {
  // put your main code here, to run repeatedly:
 F1.pull();
 F2.pull();
 F3.pull();
 F4.pull();
}
```

This is it. All the interrupts kept as fast as possible; all the task functions/procedures handled.
If you need to wait, do the job:
```cpp
 while(!neededFlag){
F1.pull();
F2.pull();
}
```
Or to delay some time? Utilize some function like:
```cpp
void DelayOnF1(uint64_t delay){
 uint64_t targetTime = delay + millis();
  while(millis() < targetTime) F1.pull();
}
```


An example:
```cpp
digitalWrite(13, HIGH); 	// sets the digital pin 13 on
//delay(1000);  		//not needed any more
DelayOnF1(1000);		// wait by doing
val = analogRead(3);  	// read the input pin
```


If you need to pass several arguments – no problem, you may use own class for a queue:
```cpp
class testClass{        // it is not used here, just like example how you may pass complex argument to your functions in queue
  public:
    int array[10]={0,0,0,0,0,0,0,0,0,0};
    float argument = 0.0;
};
fQP<testClass> F2(10);
```
Your class instances, passed to functions, should be of constant size

If you are not sure of interrupts priorities, push to different queues in each interrupt

You even may improve your job by dividing all your functions into "weight" groups:
1.	Divide all your functions into groups: fast (in one pass), slow-to-complete, and middle functions.
2.	Create a separate queue of pointers to these functions for each type.
3.	Execute the fastest functions instead of waiting inside of the middle functions.
4.	Use the medium and fast pulls for waiting inside of slow procedures.

