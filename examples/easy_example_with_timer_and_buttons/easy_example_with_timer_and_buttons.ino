#include <antirtos.h>


fQ F1(16);
fQ F2(10);
fQP<int32_t> F3(20);

void exampleFunc1(){
 
}

void exampleFunc2(){
 
}

void exampleFuncP(int32_t param){
 return param;
}

void setup() {
  // put your setup code here, to run once:
 F1.push(exampleFunc1);
 F1.push(exampleFunc2);
 F3.push(exampleFuncP, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  F1.pull();
  F2.pull();
  F3.pull();
}
