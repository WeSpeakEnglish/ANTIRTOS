// under MIT license, Aleksei Tertychnyi

#ifndef antirtos_h
#define antirtos_h

#include <cstddef>
#include <tuple>

namespace Antirtos
{

namespace Utils
{
  /* Template parameter pack for index sequence*/
  template <std::size_t... IndexList>
  struct index_sequence
  {
    /* Create an index_sequence with an additional element*/
    template <std::size_t n>
    using Append = index_sequence<IndexList..., n>;
  };

  /* Metafunction to create an index_sequence from 0 to SequenceSize-1*/
  template <std::size_t SequenceSize>
  struct make_index_sequence
  {
    using Type =
        typename make_index_sequence<SequenceSize - 1>::Type::template Append<SequenceSize - 1>;
  };

  /* Termination of the recursive metafunction*/
  template <>
  struct make_index_sequence<0u>
  {
    /* Type defintion for 0 args*/
    using Type = index_sequence<>;
  };

  /* Convenience alias*/
  template <typename... TypeList>
  using IndexSequenceFor = typename make_index_sequence<sizeof...(TypeList)>::Type;

} // namespace Utils

/// @brief Class for a given task
/// @tparam TArgs - variadic template args to have any number of templated args for the function
/// call.
template <typename... TArgs>
class Task
{
public:
  using FunctionPointer = void (*)(TArgs...); // Function pointer type
  using FunctionArgs = std::tuple<TArgs...>;

  /// @brief Construct a new task object
  Task();

  /// @brief Destroy the task object
  ~Task();

  /// @brief Copy constructor
  Task(const Task &other);

  /// @brief Copy assignment operator
  Task &operator=(const Task &other);

  /// @brief Move constructor
  Task(Task &&other) noexcept;

  /// @brief Move assignment operator
  Task &operator=(Task &&other) noexcept;

  /// @brief Sets the function to be called and the arguments to be passed at time of calling.
  /// @param f_p - function pointer
  /// @param margs - arguments to be called
  void set(FunctionPointer f_p, TArgs &&...margs);

  /// @brief Resets the function pointer to NULL.
  void reset();

  /// @brief Checks if the task is runable.
  /// @return true - it is.
  /// @return false - it is not.
  bool runable();

  /// @brief Runs the function
  /// @return int
  /// 0 - Successful run
  /// -1 - nothing to run
  int run();

  /// @brief gets function pointer
  /// @return FunctionPointer - assigned function pointer
  typename Task<TArgs...>::FunctionPointer function_pointer();

private:
  FunctionPointer func_ptr; // Function pointer
  FunctionArgs func_args;   // Tuple to store arguments

  /// @brief Helper to call the function with unpacked arguments
  /// @tparam indexes - parameter pack containing sequence of argument indexes for passing to called
  /// function
  template <std::size_t... indexes>
  void call_func(Utils::index_sequence<indexes...>);
};

template <typename... TArgs>
Task<TArgs...>::Task() : func_ptr(nullptr)
{
}

template <typename... TArgs>
Task<TArgs...>::~Task()
{
}

template <typename... TArgs>
Task<TArgs...>::Task(const Task &other) : func_ptr(other.func_ptr), func_args(other.func_args)
{
}

template <typename... TArgs>
Task<TArgs...> &Task<TArgs...>::operator=(const Task &other)
{
  if (this != &other)
  {
    func_ptr = other.func_ptr;
    func_args = other.func_args;
  }
  return *this;
}

template <typename... TArgs>
Task<TArgs...>::Task(Task &&other) noexcept
    : func_ptr(other.func_ptr), func_args(std::move(other.func_args))
{
  other.func_ptr = nullptr;
}

template <typename... TArgs>
Task<TArgs...> &Task<TArgs...>::operator=(Task &&other) noexcept
{
  if (this != &other)
  {
    func_ptr = other.func_ptr;
    func_args = std::move(other.func_args);
    other.func_ptr = nullptr;
  }
  return *this;
}

template <typename... TArgs>
void Task<TArgs...>::set(FunctionPointer f_p, TArgs &&...margs)
{
  func_ptr = f_p;
  func_args = std::make_tuple(std::forward<TArgs>(margs)...);
}

template <typename... TArgs>
void Task<TArgs...>::reset()
{
  func_ptr = nullptr;
}

template <typename... TArgs>
bool Task<TArgs...>::runable()
{
  return (func_ptr != nullptr);
}

template <typename... TArgs>
int Task<TArgs...>::run()
{
  int status = -1;

  if (func_ptr)
  {
    call_func(Utils::IndexSequenceFor<TArgs...>());
    status = 0;
  }

  return status;
}

template <typename... TArgs>
typename Task<TArgs...>::FunctionPointer Task<TArgs...>::function_pointer()
{
  return func_ptr;
}

template <typename... TArgs>
template <std::size_t... indexes>
void Task<TArgs...>::call_func(Utils::index_sequence<indexes...>)
{
  // Unpack the tuple and call the function with the arguments.
  func_ptr(std::get<indexes>(func_args)...);
}

/// @brief task queue class.
/// @tparam QSize - the maximum number of tasks the queue can hold.
/// @tparam TArgs - variadic template arguments for the tasks
template <std::size_t QSize, typename... TArgs>
class tQ
{
public:
  /// @brief Constructs task queue
  tQ();

  /// @brief Destroys task queue
  ~tQ();

  /// @brief Adds a task to the queue.
  /// @param f_p - Function pointer to be called
  /// @param margs - Arguments for the function
  /// @return int
  /// 0 - added OK
  /// 1 - Queue full
  int push(typename Task<TArgs...>::FunctionPointer f_p, TArgs &&...margs);

  /// @brief Runs the next task in the queue and resets it afterwards.
  /// @return int
  /// 0 - Successful run
  /// -1 - No tasks to run
  int pull(void);

protected:
  std::size_t front;           // front of the queue
  std::size_t back;            // back of the queue
  std::size_t count;           // number of items currently in the queue
  Task<TArgs...> tasks[QSize]; // Task queue memory block

  /// @brief pushes a constructed task
  /// @param task - task to be pushed onto queue
  /// @return int
  /// 0 - added OK
  /// 1 - Queue full
  int push(const Task<TArgs...> &task);
};

template <std::size_t QSize, typename... TArgs>
tQ<QSize, TArgs...>::tQ() : front(0), back(0), count(0)
{
}

template <std::size_t QSize, typename... TArgs>
tQ<QSize, TArgs...>::~tQ()
{
}

template <std::size_t QSize, typename... TArgs>
int tQ<QSize, TArgs...>::push(typename Task<TArgs...>::FunctionPointer f_p, TArgs &&...margs)
{
  int status = 1;

  // Check if the Queue is full
  if (count < QSize)
  {
    tasks[back].set(f_p, std::forward<TArgs>(margs)...);
    back = (back + 1) % QSize; // Circular increment
    ++count;                   // Increment the count
    status = 0;
  }

  return status;
}

template <std::size_t QSize, typename... TArgs>
int tQ<QSize, TArgs...>::pull(void)
{
  int status = 1;

  // Check if the Queue is empty
  if (count > 0)
  {
    status = tasks[front].run();
    tasks[front].reset();        // Reset the task
    front = (front + 1) % QSize; // Circular increment
    --count;                     // Decrement the count
  }

  return status;
}

template <std::size_t QSize, typename... TArgs>
int tQ<QSize, TArgs...>::push(const Task<TArgs...> &task)
{
  int status = 1;

  // Check if the Queue is full
  if (count < QSize)
  {
    tasks[back] = task;
    back = (back + 1) % QSize; // Circular increment
    ++count;                   // Increment the count
    status = 0;
  }

  return status;
}

/// @brief delayed task queue class
/// @tparam QSize - the maximum number of tasks the queue can hold.
/// @tparam TArgs - variadic template arguments for the tasks
template <std::size_t QSize, typename... TArgs>
class tQd : public tQ<QSize, TArgs...>
{
public:
  /// @brief Constructs delayed task queue
  tQd();

  /// @brief Destroys delayed task queue
  ~tQd();

  /// @brief Adds a task to the delayed task queue.
  /// @param f_p - Function pointer to be called
  /// @param margs - Arguments for the function
  /// @param delay_time - delay time in ticks
  /// @return int
  /// 0 - added OK
  /// 1 - Queue full
  int push_delayed(typename Task<TArgs...>::FunctionPointer f_p, TArgs &&...margs,
                   std::size_t delay_time);

  /// @brief removes the delayed tasks asscated with the function pointer before they're called.
  /// @param f_p - Function pointer for which ssociated tasks are to be removed.
  /// @return bool
  /// true - tasks removed
  /// false - no associated tasks to remove
  bool revoke(typename Task<TArgs...>::FunctionPointer f_p);

  /// @brief tick function, to be palced in periodic ISR - controls delay timings.
  void tick(void);

private:
  std::size_t time;                    // Variable for current time.
  std::size_t count;                   // Current count of delayed tasks
  Task<TArgs...> delayed_tasks[QSize]; // Delayed task queue memory block
  std::size_t exec_times[QSize];       // Execution times.
};

template <std::size_t QSize, typename... TArgs>
tQd<QSize, TArgs...>::tQd() : time(0)
{
}

template <std::size_t QSize, typename... TArgs>
tQd<QSize, TArgs...>::~tQd()
{
}

template <std::size_t QSize, typename... TArgs>
int tQd<QSize, TArgs...>::push_delayed(typename Task<TArgs...>::FunctionPointer f_p,
                                       TArgs &&...margs, std::size_t delay_time)
{
  int full = 1;

  if (count < QSize)
  {
    std::size_t exec_time = time + delay_time; // Calculate execution time
    std::size_t insert_index = 0;

    // Find the correct position to insert to keep tasks sorted
    while (insert_index < count && exec_times[insert_index] <= exec_time)
    {
      insert_index++;
    }

    // Shift tasks to make room for the new task
    for (std::size_t i = count; i > insert_index; --i)
    {
      delayed_tasks[i] = delayed_tasks[i - 1];
      exec_times[i] = exec_times[i - 1];
    }

    // Insert the new task
    delayed_tasks[insert_index].set(f_p, std::forward<TArgs>(margs)...);
    exec_times[insert_index] = exec_time; // Assign the execution time
    ++count;                              // Increment the count

    full = 0;
  }

  return full;
}

template <std::size_t QSize, typename... TArgs>
bool tQd<QSize, TArgs...>::revoke(typename Task<TArgs...>::FunctionPointer f_p)
{
  bool found = false; // Flag to track if any task was found and revoked

  // Iterate through the delayed tasks to find matching function pointers
  for (std::size_t i = 0; i < count;)
  {
    // Compare the function pointer of the current task with the one to revoke
    if (delayed_tasks[i].runable() && delayed_tasks[i].function_pointer() == f_p)
    {
      // Found the task to revoke
      delayed_tasks[i].reset(); // Reset the task

      // Shift remaining tasks down to fill the gap
      for (std::size_t j = i; j < count - 1; ++j)
      {
        delayed_tasks[j] = delayed_tasks[j + 1];
        exec_times[j] = exec_times[j + 1];
      }

      // Decrement the count of tasks
      --count;
      found = true; // Mark that we found and revoked a task
                    // Do not increment `i`, because we need to check the new task at this index
    }
    else
    {
      ++i; // Only increment if no task was revoked
    }
  }

  return found; // Return true if at least one task was revoked, false otherwise
}

template <std::size_t QSize, typename... TArgs>
void tQd<QSize, TArgs...>::tick(void)
{
  // Increment the current time
  ++time;

  // Check for tasks ready to run
  while (count > 0 && exec_times[0] == time)
  {
    // Pull the first task to be executed
    this->push(delayed_tasks[0]);
    delayed_tasks[0].reset(); // Reset the task

    // Shift remaining tasks down
    for (std::size_t i = 1; i < count; ++i)
    {
      delayed_tasks[i - 1] = delayed_tasks[i];
      exec_times[i - 1] = exec_times[i];
    }

    // Decrease the count
    --count;
  }
}

} // Antirtos

#endif
