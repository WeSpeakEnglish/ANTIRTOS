<img src="https://github.com/WeSpeakEnglish/images/blob/main/logo_antirtos.png" width="300">


# ANTIRTOS 
### *No RTOS need, you will see...*
ANTIRTOS is an ultra-lightweight, universal C++ library designed for task management in IoT and embedded applications. It is coded in a single, small file, making it incredibly easy to integrate into your projects. ANTIRTOS aims to deliver a robust, secure, and efficient library for task management. With its focus on modularity and scalability, ANTIRTOS is suitable for a variety of devices, from simple microcontrollers to complex embedded systems.
It is developed especially for embedded systems where the complexity and overhead of an RTOS are not justified.

### Benifits:
1.	Interrupts kept fast and controllable, no blocking anymore.
2.	Easy to debug and understand. 
3.	Get an easy way of multitasking.
4.	No dummy waiting/blockings. Wait by doing!
5.	No need to have a deal with critical sections/mutexes/semaphores, no tasks stack memory overflows. Keep your project based on straight forward architecture

Get rid of complexity!   
Allow your MCU to perform many tasks while using a very small amount of microcontroller memory. Manage function pointer queues in a simple way by simply placing your tasks there and pulling them from a main loop. This approach allows you to keep interrupts fast and at the same time control multitasking in a simple and transparent way. Pass arguments to you function, they will be saved in separate queue and handled.

### Let’s explain a bit on examples.

0. To start using the library - include the header:
```cpp
#include <antirtos.h>
```

1.	Create queues in an easy way
```cpp
fQ F1(4); // first queue is 4 elements(function pointers) long
fQ F2(4); // second queue is 4 elements(function pointers) long
fQ F3(8); // third queue is 8 elements(function pointers) long
fQP<int32_t> F4(3); // fourth queue is 3 elements(function pointers)
		    // functions are receiving int32_t argument
```


2.	Wherever you want, just push your function pointers (and arguments if they needed).  

```cpp
void button1Interrupt(){
	F1.push(Below used following functions); // void dealAssociatedButton1() – is your task for this button
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
	F4.push(dealAssociatedTimer2, yourIntArg);  // void dealAssociatedTimer2(int yourIntArg) – 
                                                // is your task for this timer2 event and the variable yourIntArg
                                                //  passed like an argument 
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
Or to delay for some time? Utilize a function like:
```cpp
void DelayOnF1(uint64_t delay){
	uint64_t targetTime = delay + millis();
	while(millis() < targetTime) 
		F1.pull();
}
```


An example:
```cpp
digitalWrite(13, HIGH); 	// sets the digital pin 13 on
//delay(1000);  		//not needed any more
DelayOnF1(1000);		// wait by doing
val = analogRead(3);  		// read the input pin
```


If you need to pass several arguments – no problem, you may use your own class for a queue:
```cpp
class testClass{        // it is not used here, just like example how you may pass complex argument to your functions in queue
  public:
    int array[10]={0,0,0,0,0,0,0,0,0,0};
    float argument = 0.0;
};
fQP<testClass> F2(10);
```
Instances of your class passed to functions must be of constant size.

### Delayed functions
Do you need just to delay some function from execution? Do not wait any more!
Initialize:
```cpp
 del_fQ F5(8); // 'delayed' queue

 ```
put where you want (here example of 2 functions put into queue):
```cpp
 F5.push_delayed(your_func_1, 1000); // function your_func_1() will be delayed for 1000 'ticks'
 F5.push_delayed(your_func_2, 2000); // function your_func_2() will be delayed for 2000 'ticks'
 ```
 
 in main loop (or other periodic loop) just need to:
 ```cpp
 void loop() {
 .......
 F5.pull(); //execute in loop just this super fast function;
 }
 ```
 in some timer or periodic function:
  ```cpp
	F5.tick(); // execute for 'ticks' in timer so the queue class instance will know then to initiate execution
  ```
  
  ### Delayed functions with parameters
Do you need to delay the execution of a function that receives a parameter? With ANTIRTOS you can do it easily!
Initialize:
```cpp
 del_fQP<float> F6(8); // // maximum 8 'delayed' functions pointers with parameters in queue

 ```
put where you want (here example of 2 functions put into queue):
```cpp
 F6.push_delayed(your_func_1, 3.14, 1000); // function your_func_1(3.14) will be delayed for 1000 'ticks'
 F6.push_delayed(your_func_2, 3.15, 2000); // function your_func_2(3.15) will be delayed for 2000 'ticks'
 ```
 
 in main loop (or other periodic loop) just need to:
 ```cpp
 void loop() {
 .......
 F6.pull(); //execute in loop just this super fast function;
 }
 ```
 in some timer or periodic function:
  ```cpp
	F6.tick(); // execute for 'ticks' in timer so the queue class instance will know then to initiate execution
  ```

  ### Revocation function from conveyers
  You may easily revoke your tasks from delayed functional queues like following:
   ```cpp
  F5.revoke(yourTask); // revoke function (all of the same if there are several of them) from the F5 queue
  ```
Kindly find an example of usage revocation on [Wokwi](https://wokwi.com/projects/411101121732784129)!

That's it. Enjoy! 
 

If you are not sure of interrupts priorities, push to different queues in each interrupt

You even may improve your job by dividing all your functions into "weight" groups:
1.	Divide all your functions into groups: fast (in one pass), slow-to-complete, and middle functions.
2.	Create a separate queue of pointers to these functions for each type.
3.	Execute the fastest functions instead of waiting inside of the middle functions.
4.	Use the medium and fast pulls for waiting inside of slow procedures.

Try it on [Wokwi](https://wokwi.com/projects/410932957331738625)!

