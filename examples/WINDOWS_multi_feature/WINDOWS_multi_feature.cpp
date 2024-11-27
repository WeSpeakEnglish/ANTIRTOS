#include "WINDOWS_multi_feature.hpp"
#include "antirtos.h"
#include <iostream>

typedef struct my_object_t
{
  int first_arg;
  float second_arg;
} my_object_t;

antirtos::TaskQ<2> tq_no_parameter;
void PrintNoParameter()
{
  std::cout << "No Parameter" << std::endl;
}

antirtos::TaskQ<2, int> tq_one_parameter;
void PrintOneParameter(int x)
{
  std::cout << "One Parameter: " << x << std::endl;
}

antirtos::TaskQ<2, int, float> tq_two_parameters;
void PrintTwoParameters(int x, float y)
{
  std::cout << "Two Parameters: " << x << " & " << y << std::endl;
}

antirtos::TaskQd<2, int> tq_del_one_parameter;
void PrintDelayedParameter(int x)
{
  std::cout << "Delayed Call, Parameter: " << x << std::endl;
}

antirtos::TaskQd<2, my_object_t> tq_del_object_parameter;
void PrintDelayedObjectParameter(my_object_t x)
{
  std::cout << "Delayed Call" << std::endl;
  std::cout << "Object Parameter First Arg: " << x.first_arg << std::endl;
  std::cout << "Object Parameter Second Arg: " << x.second_arg << std::endl;
}

int main()
{
  // Instance Timers
  Timer timer;
  Timer tick_timer;

  // Install tick timer
  tick_timer.start(50,
                   []()
                   {
                     tq_del_one_parameter.tick();
                     tq_del_object_parameter.tick();
                   });

  // Run timer every second passing lambda
  timer.start(1000,
              []()
              {
                tq_no_parameter.push(PrintNoParameter);
                tq_one_parameter.push(PrintOneParameter, 1);
                tq_two_parameters.push(PrintTwoParameters, 2, 3.14f);
              });

  // Call in 2 seconds
  tq_del_one_parameter.push_delayed(PrintDelayedParameter, 10, 40);
  // Call in 1 second
  tq_del_object_parameter.push_delayed(PrintDelayedObjectParameter, {10, 123.456}, 20);

  while (1)
  {

    // Get all functions in no param task queue
    tq_no_parameter.pull_all();

    // Get all functions in one param task queue
    tq_one_parameter.pull_all();

    // Get all functions in two param task queue
    tq_two_parameters.pull_all();

    // Get all functions in one param delayed task queue
    tq_del_one_parameter.pull_all();

    // Get all functions in object param delayed task queue
    tq_del_object_parameter.pull_all();
  }

  return 0;
}
