#include "WIN32_static_helper.hpp"
#include "antirtos.h"
#include <iostream>

typedef struct my_object_t
{
  int first_arg;
  float second_arg;
} my_object_t;

Antirtos::tQ<2> tq_no_parameter;
void PrintNoParameter()
{
  std::cout << "No Parameter" << std::endl;
}

Antirtos::tQ<2, int> tq_one_parameter;
void PrintOneParameter(int x)
{
  std::cout << "One Parameter: " << x << std::endl;
}

Antirtos::tQ<2, int, float> tq_two_parameters;
void PrintTwoParameters(int x, float y)
{
  std::cout << "Two Parameters: " << x << " & " << y << std::endl;
}

Antirtos::tQd<2, int> tq_del_one_parameter;
void PrintDelayedParameter(int x)
{
  std::cout << "Delayed Call, Parameter: " << x << std::endl;
}

Antirtos::tQd<2, my_object_t> tq_del_object_parameter;
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
    int err = 0;

    // Get all functions in no param task queue
    do
    {
      err = tq_no_parameter.pull();
    } while (0 == err);

    // Get all functions in one param task queue
    do
    {
      err = tq_one_parameter.pull();
    } while (0 == err);

    // Get all functions in two param task queue
    do
    {
      err = tq_two_parameters.pull();
    } while (0 == err);

    // Get all functions in one param delayed task queue
    do
    {
      err = tq_del_one_parameter.pull();
    } while (0 == err);

    // Get all functions in object param delayed task queue
    do
    {
      err = tq_del_object_parameter.pull();
    } while (0 == err);
  }

  return 0;
}
