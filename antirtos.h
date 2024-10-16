// under MIT license, Aleksei Tertychnyi

#ifndef antirtos_h
#define antirtos_h
/// @brief class for finctional pointers queue without parameters
class fQ {
  public:
    typedef void(*fP)(void);
    fQ(int sizeQ);
    ~fQ();
    int push(fP);
    int pull(void);
  private:
    int first;
    volatile int last;
    int length;
    fP * fQueue;
};

fQ::fQ(int sizeQ) { // initialization of Queue (constructor)
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

/// @brief class for functional pointers queue with supporting of transfer of parameters
template <typename T>
class fQP {
  public:
    fQP(int sizeQ);
    ~fQP();
    int push(void (*pointerF)(T), T parameterQ);
    int pull();
  private:
    int first;
    volatile int last;
    int length;
    typedef void (*fP)(T);
    fP* FP_Queue;
    T* PARAMS_array;
};

template <typename T>
fQP<T>::fQP(int sizeQ) { // constructor
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

/// @brief delayed functional pointers queue without parameters, only time delay parameter
class del_fQ {
  public:
    typedef void(*fP)(void);
    del_fQ(int sizeQ);				// constructor
    ~del_fQ();                // destructor
    int push_delayed(fP pointerF, unsigned long delayTime); //push delayed
    int push(fP pointerF);                                  //direct push without delay, use with care
    void tick(void);       							//tick() - put it into periodic function or interrupt ISR
    int pull(void);											//pull function pointer and execute
    int revoke(fP pointerF); 						//revoke the function pointer/s from the queue (will be not executed)
  private:
    int first;
    volatile int last;
    int length;
    unsigned long time;
    fP * fQueue;  					//'ready' for execution functions will be put in this queue
    fP * del_fQueue;             	// delayed functions queue
    bool * execArr;             	//is need to be executed?
    unsigned long * execTime;    	//execution time arr
};

del_fQ::del_fQ(int sizeQ) { // constructor
  fQueue = new fP[sizeQ];
  del_fQueue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  execTime = new unsigned long[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  for (unsigned int i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
};

del_fQ::~del_fQ() { // destructor
  delete [] fQueue;
  delete [] del_fQueue;
  delete [] execArr;
  delete [] execTime;
};

int del_fQ::push_delayed(fP pointerF, unsigned long delayTime) { // push element from the queue
  bool fullQ = true;                                      // is Queue full?
  for (unsigned int i = 0; i < length; i++) {
    if (!execArr[i] ) {
      del_fQueue[i] = pointerF;                          // put pointer into exec queue
      execArr[i] = true;                                 // true flag for execution
      execTime[i] = time + delayTime;                    //calc execution time, no worry if overload
      fullQ = false;
      break;
    }
  }
  if (fullQ) return 1;
  return 0;
};

void del_fQ::tick(void) {    // tick method to provide delay functionality, put it into periodic routine
  static unsigned int i = 0 ;  //uses in search cycle every tick
  for (i = 0; i < length; i++) {
    if (execTime[i] == time)
      if (execArr[i]) {
        push(del_fQueue[i]);  // bump into normal queue part of delayed Queue
        execArr[i] = false;
      }
  }
  time++;
}

int del_fQ::revoke(fP pointerF) {  // revokation of task from the queue in case you do not need it any more
  int result = 1;
  for (int i = 0; i < length; i++) {
    if (del_fQueue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  return result;
}

int del_fQ::push(fP pointerF) { // push element from the queue
  if ((last + 1) % length == first) {
    return 1;
  }
  fQueue[last++] = pointerF;
  last = last % length;
  return 0;
};

int del_fQ::pull(void) { // pull element from the queue
  if (last != first) {
    fQueue[first++]();
    first = first % length;
    return 0;
  }
  else {
    return 1;
  }
};


/// @brief delayed functional pointers queue with parameters
template <typename T>
class del_fQP {
  public:
    typedef void (*fP)(T);
    int push(void (*pointerF)(T), T parameterQ); //push directly (without delay) use with care here
    del_fQP(int sizeQ);
    ~del_fQP();
    int push_delayed(void (*pointerF)(T), T parameterQ, unsigned long delayTime); //push delayed
    void tick(void); //tick() - put it into periodic function or interrupt ISR
    int revoke(void (*pointerF)(T)); // revoke the function pointer/s from the queue
    int pull();
  private:
    int first;
    volatile int last;
    int length;
    unsigned long time;
    fP * FP_Queue;						// function pointers queue
    fP * del_FP_Queue;                  // delayed function pointers
    bool * execArr;                     // is need to be executed?
    unsigned long * execTime;            // execution time array
    T* PARAMS_array;					// parameters for functions ready to execute
    T* delayed_PARAMS_array;			// parameters for delayed functions

};

template <typename T>
del_fQP<T>::del_fQP(int sizeQ) {
  FP_Queue = new fP[sizeQ];
  del_FP_Queue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  PARAMS_array = new T[sizeQ];
  delayed_PARAMS_array = new T[sizeQ];
  execTime = new unsigned long[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  for (unsigned int i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
}

template <typename T>
del_fQP<T>::~del_fQP() {
  delete[] FP_Queue;
  delete[] del_FP_Queue;
  delete[] PARAMS_array;
  delete[] delayed_PARAMS_array;
  delete [] execArr;
  delete [] execTime;
}

template <typename T>
int del_fQP<T>::push(void (*pointerF)(T), T parameterQ) {
  if ((last + 1) % length == first) return 1;
  FP_Queue[last] = pointerF;
  PARAMS_array[last] = parameterQ;
  last = (last + 1) % length;
  return 0;
}

template <typename T>
int del_fQP<T>::push_delayed(void (*pointerF)(T), T parameterQ, unsigned long delayTime) {
  bool fullQ = true;                                              // is Queue full?
  for (unsigned int i = 0; i < length; i++) {
    if (!execArr[i] ) {
      del_FP_Queue[i] = pointerF;                              // put function pointer into exec queue
      delayed_PARAMS_array[i] = parameterQ;                    // put parameter into exec queue
      execArr[i] = true;                                       // true flag for execution
      execTime[i] = time + delayTime;                          //calc execution time, no worry if overload
      fullQ = false;
      break;
    }
  }
  if (fullQ) return 1;
  return 0;
}

template <typename T>
void del_fQP<T>::tick(void) {
  static unsigned int i = 0 ;  //uses in search cycle every tick
  for (i = 0; i < length; i++) {
    if (execTime[i] == time)
      if (execArr[i]) {
        push(del_FP_Queue[i], delayed_PARAMS_array[i]); // bump into normal queue part of delayed Queue
        execArr[i] = false;
      }
  }
  time++;
}
template <typename T>
int del_fQP<T>::revoke(void (*pointerF)(T)) {
  int result = 1;
  for (int i = 0; i < length; i++) {
    if (del_FP_Queue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  return result;
}

template <typename T>
int del_fQP<T>::pull() {
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
