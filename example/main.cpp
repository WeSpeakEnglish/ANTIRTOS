#include <iostream>
#include <chrono>
#include <random>
#include <future>
#include <thread>
#include <unistd.h>
#include "nortos.h"

using namespace std;

random_device dev;
mt19937 rng(dev());
uniform_int_distribution<mt19937::result_type> dist1_1000(1,1000);


fQ F1(16);
fQ F2(12);
fQ F3(18);
fQP<int32_t> F4(20);

class Params
{
  public:
    int a = 10;
    int b = 20;
    int c = 30;
  string str="Test";

};

fQP<Params> F5(22);

void SomeTestFunctionOne(void){
  static int i = 0;
  cout << "SomeTestFunctionOne #"<< i++ << endl;
}

void SomeTestFunctionTwo(void){
  static int i = 0;
  cout << "SomeTestFunctionTwo #"<< i++ << endl;
}

void SomeTestFunctionThree(void){
  static int i = 0;
  cout << "SomeTestFunctionThree #"<< i++ << endl;
}

void SomeTestFunctionFour(int32_t parameter){
  static int i = 0;
  cout << "SomeTestFunctionFour #"<< i++ << "  passed parameter:" << parameter << endl;
}

void SomeTestFunctionFive(Params parameter){
  static int i = 0;

  cout << "SomeTestFunctionFour #"<< i++ << "  passed parameters: a = " << parameter.a << ", b = " << parameter.b << ", c = " << parameter.c << ",str = " << parameter.str << " sizeof passed object: " << sizeof(parameter) <<endl;
}

volatile int FlagStop = 0;

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;


void Timer1() {
  while(!FlagStop){
    F1.push(SomeTestFunctionOne);
    sleep_for(milliseconds(dist1_1000(rng)));
  }
}

void Timer2() {
  while(!FlagStop){
    F2.push(SomeTestFunctionTwo);
    sleep_for(milliseconds(dist1_1000(rng)));
  }
}

void Timer3() {
  while(!FlagStop){
    F3.push(SomeTestFunctionThree);
    sleep_for(milliseconds(dist1_1000(rng)));
  }
}

void Timer4() {
  while(!FlagStop){
    F4.push(SomeTestFunctionFour,42);
    sleep_for(milliseconds(dist1_1000(rng)));
  }
}

void Timer5() {
  Params params_to_pass;
  
  while(!FlagStop){
    F5.push(SomeTestFunctionFive,params_to_pass);
    sleep_for(milliseconds(dist1_1000(rng)));
  }
}

int main()
{
  auto Q_one = async(launch::async, &Timer1);
  auto Q_two = async(launch::async, &Timer2);
  auto Q_three = async(launch::async, &Timer3);
  auto Q_four = async(launch::async, &Timer4);
  auto Q_five = async(launch::async, &Timer5);

  for(;;){
      F1.pull();
      F2.pull();
      F3.pull();
      F4.pull();
      F5.pull();
      usleep(10000);
  }

  return 0;
}
