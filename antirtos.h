// under MIT license, Aleksei Tertychnyi

#ifndef antirtos_h

/**
 * @class fQ
 * @brief A queue-based task scheduler for function pointers without parameters.
 *
 * This class provides a mechanism to manage and execute tasks represented as function pointers.
 * Tasks are executed in the order they are added to the queue.
 */
class fQ {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type that takes no parameters.
     */
    typedef void (*fP)(void);

    /**
     * @brief Constructor to initialize the queue with a specified size.
     * @param sizeQ The maximum number of tasks the queue can hold.
     */
    fQ(unsigned char sizeQ);

    /**
     * @brief Destructor to clean up dynamically allocated resources.
     */
    ~fQ();

    /**
     * @brief Pushes a function pointer into the queue.
     * @param pointerF The function pointer to be added to the queue.
     * @return 0 if the function was successfully added; 1 if the queue is full.
     */
    int push(fP pointerF);

    /**
     * @brief Pulls the next function from the queue and executes it.
     * @return 0 if a function was successfully executed; 1 if the queue is empty.
     */
    int pull(void);

  private:
    unsigned char first;                            ///< Index of the first element in the queue.
    volatile unsigned char last;                    ///< Index of the last element in the queue.
    unsigned char length;                           ///< Maximum size of the queue.
    fP* fQueue;                           ///< Queue for function pointers.
};

fQ::fQ(unsigned char sizeQ) { // initialization of Queue (constructor)
  fQueue = new fP[sizeQ];
  last = 0;
  first = 0;
  length = sizeQ;
};

fQ::~fQ() { // destructor
  delete [] fQueue;
};

int fQ::push(fP pointerF) { // push element from the queue
  if ((last + 1) % length == first) {
    return 1;
  }
  fQueue[last++] = pointerF;
  last = last % length;
  return 0;
};

int fQ::pull(void) { // pull element from the queue
  if (last != first) {
    fQueue[first++]();
    first = first % length;
    return 0;
  }
  else {
    return 1;
  }
};

/**
 * @class fQP
 * @brief A queue-based task scheduler for function pointers with parameter support.
 *
 * This template class allows function pointers with associated parameters to be queued
 * for execution. Tasks are executed in the order they are added.
 *
 * @tparam T The type of the parameter passed to the function pointers.
 */
template <typename T>
class fQP {
  public:
    /**
     * @brief Constructor to initialize the queue with a specified size.
     * @param sizeQ The maximum number of tasks the queue can hold.
     */
    fQP(unsigned char sizeQ);

    /**
     * @brief Destructor to clean up dynamically allocated resources.
     */
    ~fQP();

    /**
     * @brief Pushes a function pointer and its parameter into the queue.
     * @param pointerF The function pointer to be added to the queue.
     * @param parameterQ The parameter associated with the function pointer.
     * @return 0 if the function was successfully added; 1 if the queue is full.
     */
    int push(void (*pointerF)(T), T parameterQ);

    /**
     * @brief Pulls the next function from the queue and executes it with its parameter.
     * @return 0 if a function was successfully executed; 1 if the queue is empty.
     */
    int pull();

  private:
    unsigned char first;                            ///< Index of the first element in the queue.
    volatile unsigned char last;                    ///< Index of the last element in the queue.
    unsigned char length;                           ///< Maximum size of the queue.
    typedef void (*fP)(T);                ///< Function pointer type that accepts a parameter of type T.
    fP* FP_Queue;                         ///< Queue for function pointers.
    T* PARAMS_array;                      ///< Queue for parameters associated with the function pointers.
};


template <typename T>
fQP<T>::fQP(unsigned char sizeQ) { // constructor
  FP_Queue = new fP[sizeQ];
  PARAMS_array = new T[sizeQ];
  last = 0;
  first = 0;
  length = sizeQ;
}

template <typename T>
fQP<T>::~fQP() {   //destructor
  delete[] FP_Queue;
  delete[] PARAMS_array;
}

template <typename T>
int fQP<T>::push(void (*pointerF)(T), T parameterQ) {   //push your task into queue
  if ((last + 1) % length == first) return 1;
  FP_Queue[last] = pointerF;
  PARAMS_array[last] = parameterQ;
  last = (last + 1) % length;
  return 0;
}

template <typename T>
int fQP<T>::pull() {  // pulls task and parameters from the queue and execute
  fP pullVar;
  if (last != first) {
    T Params = PARAMS_array[first];
    pullVar = FP_Queue[first];
    first = (first + 1) % length;
    pullVar(Params);
    return 0;
  }
  else {
    return 1;
  }
}

/**
 * @class del_fQ
 * @brief A queue-based task scheduler for function pointers without parameters, with optional delayed execution.
 *
 * This class provides a mechanism to manage function pointers with optional delays in their execution. 
 * Tasks can be executed immediately or scheduled for future execution based on a time delay.
 *
 */
template <typename Tm = unsigned long>
class del_fQ_t {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type that takes no parameters.
     */
    typedef void (*fP)(void);

    /**
     * @brief Constructor to initialize the queue with a specified size.
     * @param sizeQ The maximum number of tasks the queue can hold.
     */
    del_fQ_t(unsigned char sizeQ);

    /**
     * @brief Destructor to clean up dynamically allocated resources.
     */
    ~del_fQ_t();

    /**
     * @brief Pushes a function pointer into the delayed queue for future execution.
     * @param pointerF The function pointer to be added to the delayed queue.
     * @param delayTime The delay in ticks before the function is executed.
     * @return 0 if the function was successfully added; 1 if the delayed queue is full.
     */
    int push_delayed(fP pointerF, Tm delayTime);

    /**
     * @brief Pushes a function pointer into the queue for immediate execution.
     * @param pointerF The function pointer to be added to the queue.
     * @return 0 if the function was successfully added; 1 if the queue is full.
     */
    int push(fP pointerF);

    /**
     * @brief Periodic function to manage delayed tasks. Call this method in an ISR or main loop.
     */
    void tick(void);

    /**
     * @brief Pulls the next function from the queue and executes it.
     * @return 0 if a function was successfully executed; 1 if the queue is empty.
     */
    int pull(void);

    /**
     * @brief Revokes a function pointer from the delayed queue.
     * @param pointerF The function pointer to be removed.
     * @return 0 if the function pointer was successfully revoked; 1 if it was not found.
     */
    int revoke(fP pointerF);

  private:
    unsigned char first;                            ///< Index of the first element in the queue.
    volatile unsigned char last;                    ///< Index of the last element in the queue.
    unsigned char length;                           ///< Maximum size of the queue.
    Tm time;                   ///< Current tick count for managing delays.
    fP *fQueue;                           ///< Queue for immediate function pointers.
    fP *del_fQueue;                       ///< Queue for delayed function pointers.
    bool *execArr;                        ///< Execution flags for delayed tasks.
    Tm *execTime;              ///< Execution times for delayed tasks.
};

template <typename Tm>
del_fQ_t<Tm>::del_fQ_t(unsigned char sizeQ) {       // constructor
  fQueue = new fP[sizeQ];
  del_fQueue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  execTime = new Tm[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  for (unsigned int i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
};

template <typename Tm>
del_fQ_t<Tm>::~del_fQ_t() { // destructor
  delete [] fQueue;
  delete [] del_fQueue;
  delete [] execArr;
  delete [] execTime;
};

template <typename Tm>
int del_fQ_t<Tm>::push_delayed(fP pointerF, Tm delayTime) {    // push element from the queue
  bool fullQ = true;                                                // is Queue full?
  for (unsigned char i = 0; i < length; i++) {
    if (!execArr[i] ) {
      del_fQueue[i] = pointerF;                                    // put pointer into exec queue
      execArr[i] = true;                                           // true flag for execution
      execTime[i] = time + delayTime;                              //calc execution time, no worry if overload
      fullQ = false;
      break;
    }
  }
  if (fullQ) return 1;
  return 0;
};

template <typename Tm>
void del_fQ_t<Tm>::tick(void) {                                          // tick method to provide delay functionality, put it into periodic routine
									                                     // uses in search cycle every tick
  for (unsigned char i = 0; i < length; i++) {
    if (execTime[i] == time)
      if (execArr[i]) {
        push(del_fQueue[i]);                                      // bump into normal queue part of delayed Queue
        execArr[i] = false;
      }
  }
  time++;
}

template <typename Tm>
int del_fQ_t<Tm>::revoke(fP pointerF) {  // revocation of task from the queue in case you do not need it any more
  int result = 1;
  for (unsigned char i = 0; i < length; i++) {
    if (del_fQueue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  return result;
}

template <typename Tm>
int del_fQ_t<Tm>::push(fP pointerF) {   // push element from the queue
  if ((last + 1) % length == first) {
    return 1;
  }
  fQueue[last++] = pointerF;
  last = last % length;
  return 0;
};

template <typename Tm>
int del_fQ_t<Tm>::pull(void) {         // pull element from the queue
  if (last != first) {
    fQueue[first++]();
    first = first % length;
    return 0;
  }
  else {
    return 1;
  }
};

class del_fQ : public del_fQ_t<unsigned long> {
public:
    del_fQ(unsigned char sizeQ) : del_fQ_t<unsigned long>(sizeQ) {}
};

/**
 * @class del_fQP
 * @brief A queue-based task scheduler for function pointers with optional delayed execution.
 *
 * This template class manages function pointers and their execution. It supports both immediate
 * and delayed execution of tasks with associated parameters. The class is suitable for
 * embedded applications with constrained resources.
 *
 * @tparam T The type of the parameter passed to the function pointers.
 */
template <typename T, typename Tm = unsigned long>
class del_fQP {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type that accepts a parameter of type T.
     */
    typedef void (*fP)(T);

    /**
     * @brief Constructor to initialize the queue with a given size.
     * @param sizeQ The maximum number of tasks the queue can hold.
     */
    del_fQP(unsigned char sizeQ);

    /**
     * @brief Destructor to clean up dynamically allocated resources.
     */
    ~del_fQP();

    /**
     * @brief Pushes a function pointer with its parameter into the queue for immediate execution.
     * @param pointerF The function pointer to be pushed.
     * @param parameterQ The parameter associated with the function pointer.
     * @return 0 if the function was successfully added; 1 if the queue is full.
     */
    int push(void (*pointerF)(T), T parameterQ);

    /**
     * @brief Pushes a function pointer with its parameter into the delayed queue for future execution.
     * @param pointerF The function pointer to be added to the delayed queue.
     * @param parameterQ The parameter associated with the function pointer.
     * @param delayTime The delay in ticks before the function is executed.
     * @return 0 if the function was successfully added; 1 if the delayed queue is full.
     */
    int push_delayed(void (*pointerF)(T), T parameterQ, Tm delayTime);

    /**
     * @brief Periodic function to manage delayed tasks. Call this method in an ISR or main loop.
     */
    void tick(void);

    /**
     * @brief Revokes a function pointer from the delayed queue.
     * @param pointerF The function pointer to be removed.
     * @return 0 if the function pointer was successfully revoked; 1 if it was not found.
     */
    int revoke(void (*pointerF)(T));

    /**
     * @brief Pulls the next function from the queue and executes it.
     * @return 0 if a function was successfully executed; 1 if the queue is empty.
     */
    int pull();

  private:
    unsigned char first;                            ///< Index of the first element in the queue.
    volatile unsigned char last;                    ///< Index of the last element in the queue.
    unsigned char length;                           ///< Maximum size of the queue.
    Tm time;                   			  ///< Current tick count for managing delays.
    fP *FP_Queue;                         ///< Queue for immediate function pointers.
    fP *del_FP_Queue;                     ///< Queue for delayed function pointers.
    bool *execArr;                        ///< Execution flags for delayed tasks.
    Tm *execTime;              			  ///< Execution times for delayed tasks.
    T *PARAMS_array;                      ///< Parameters for immediate tasks.
    T *delayed_PARAMS_array;              ///< Parameters for delayed tasks.
};

template <typename T, typename Tm>
del_fQP<T, Tm>::del_fQP(unsigned char sizeQ) {
  FP_Queue = new fP[sizeQ];
  del_FP_Queue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  PARAMS_array = new T[sizeQ];
  delayed_PARAMS_array = new T[sizeQ];
  execTime = new Tm[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  for (unsigned char i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
}

template <typename T, typename Tm>
del_fQP<T, Tm>::~del_fQP() {
  delete[] FP_Queue;
  delete[] del_FP_Queue;
  delete[] PARAMS_array;
  delete[] delayed_PARAMS_array;
  delete [] execArr;
  delete [] execTime;
}

template <typename T, typename Tm>
int del_fQP<T, Tm>::push(void (*pointerF)(T), T parameterQ) {
  if ((last + 1) % length == first) return 1;
  FP_Queue[last] = pointerF;
  PARAMS_array[last] = parameterQ;
  last = (last + 1) % length;
  return 0;
}

template <typename T, typename Tm>
int del_fQP<T, Tm>::push_delayed(void (*pointerF)(T), T parameterQ, Tm delayTime) {
  bool fullQ = true;                                           // is Queue full?
  for (unsigned char i = 0; i < length; i++) {
    if (!execArr[i] ) {
      del_FP_Queue[i] = pointerF;                              // put function pointer into exec queue
      delayed_PARAMS_array[i] = parameterQ;                    // put parameter into exec queue
      execArr[i] = true;                                       // true flag for execution
      execTime[i] = time + delayTime;                          // calc execution time, no worry if overload
      fullQ = false;
      break;
    }
  }
  if (fullQ) return 1;
  return 0;
}

template <typename T, typename Tm>
void del_fQP<T, Tm>::tick(void) {
  for (unsigned char i = 0; i < length; i++) {
    if (execTime[i] == time)
      if (execArr[i]) {
        push(del_FP_Queue[i], delayed_PARAMS_array[i]);       // bump into normal queue part of delayed Queue
        execArr[i] = false;
      }
  }
  time++;
}
template <typename T, typename Tm>
int del_fQP<T, Tm>::revoke(void (*pointerF)(T)) {  // revocation method, revokes 
  int result = 1;
  for (unsigned char i = 0; i < length; i++) {
    if (del_FP_Queue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  return result;
}

template <typename T, typename Tm>
int del_fQP<T, Tm>::pull() {
  fP pullVar;
  if (last != first) {
    T Params = PARAMS_array[first];
    pullVar = FP_Queue[first];
    first = (first + 1) % length;
    pullVar(Params);
    return 0;
  }
  else {
    return 1;
  }
}

#endif
