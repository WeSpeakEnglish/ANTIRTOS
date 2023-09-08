// here are ANTIRTOS two classes 
// -first fQ for functions without parameters, the second with parameter 
// you could easily initialize your queue like fQ F1(10); // 10 - here is your desired lenght of task queue, F1 - you may replace with your name - name of queue 
// in case you plan to pass parameters to functions too: fQp<yourClass> F2(10); // yourClass -could be your defined class or standard class (ex int, uint16_t... etc ), 10 - here is lenght of task queue, F2 - you may replace with your name - name of queue  
// push method is pushing your function (or function and parameter ), example: F1.push(yourFunctionName); 
// in case we have functions with parameters: F2.push(yourFunctionName, parameter);
// pull function pulling and execute the task from queue, example: F1.pull(); F2.pull;
// under MIT license, Aleksei Tertychnyi

#ifndef antirtos_h
#define antirtos_h

/// @brief class for finctional pointers queue without parameters
class fQ {
private:
    volatile int first;
    volatile int last;
    int length;
	typedef void(*fP)(void);
    fP * fQueue;
public:
    fQ(int sizeQ);
    ~fQ();
    int push(fP);
    int pull(void);
};

fQ::fQ(int sizeQ){ // initialization of Queue
  fQueue = new fP[sizeQ];
  last = 0;
  first = 0;
  length = sizeQ;
};

fQ::~fQ(){ // initialization of Queue
  delete [] fQueue;
};

int fQ::push(fP pointerF){ // push element from the queue
  if ((last+1)%length == first){
    return 1;
  }
  fQueue[last++] = pointerF;
  last = last%length;
  return 0;
};

int fQ::pull(void){ // pull element from the queue
  if (last != first){
  fQueue[first++]();
  first = first%length;
  return 0;
  }
  else{
   return 1;
  }
};

/// @brief class for finctional pointers queue with supporting of transfer of parameters
template <typename T>
class fQP {
private:
    volatile int first;
    volatile int last;
    int length;
    typedef void (*fP)(T);
    fP* FP_Queue;
    T* PARAMS_array;

public:
    fQP(int sizeQ);
    ~fQP();
    int push(void (*pointerQ)(T), T parameterQ);
    int pull();
};

template <typename T>
fQP<T>::fQP(int sizeQ) {
    FP_Queue = new fP[sizeQ];
    PARAMS_array = new T[sizeQ];
    last = 0;
    first = 0;
    length = sizeQ;
}

template <typename T>
fQP<T>::~fQP() {
    delete[] FP_Queue;
    delete[] PARAMS_array;
}

template <typename T>
int fQP<T>::push(void (*pointerQ)(T), T parameterQ) {
    if ((last + 1) % length == first) return 1;
    FP_Queue[last] = pointerQ;
    PARAMS_array[last] = parameterQ;
    last = (last + 1) % length;
    return 0;
}

template <typename T>
int fQP<T>::pull() {
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