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
    int first;
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

/// @brief class for functional pointers queue with supporting of transfer of parameters
template <typename T>
class fQP {
private:
    int first;
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

/// @brief delayed functional pointers queue without parameters, only time delay parameter
class del_fQ{
private:
    int first;
    volatile int last;
    int length;
    unsigned long time;
    typedef void(*fP)(void);
    fP * fQueue;
    fP * del_fQueue;             // delayed functions
    bool * execArr;             //is need to be executed?
    unsigned int * execTime;    //execution time arr 
    int push(fP);
public:
    del_fQ(int sizeQ);
    ~del_fQ();
    int push_delayed(fP pointerF, unsigned long delayTime);
    void tick(void);       
    int pull(void);
};

del_fQ::del_fQ(int sizeQ){ // initialization of Queue
  fQueue = new fP[sizeQ];
  del_fQueue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  execTime = new unsigned long[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  for(unsigned int i = 0; i < sizeQ; i++){
    execArr[i] = false;
  }
  length = sizeQ;
};

del_fQ::~del_fQ(){ // initialization of Queue
  delete [] fQueue;
  delete [] del_fQueue;
  delete [] execArr;
  delete [] execTime;
};

int del_fQ::push_delayed(fP pointerF, unsigned long delayTime){ // push element from the queue
  
  bool fullQ = true;                                      // is Queue full?
     for(unsigned int i = 0; i < length; i++){
      if (!execArr[i] ){
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

void del_fQ::tick(void){
  static unsigned int i = 0 ;  //uses in search cycle every tick
   for(i=0; i < length; i++){
     if(execTime[i] == time)
      if(execArr[i]){
       push(del_fQueue[i]);  // bump into normal queue part of delayed Queue
       execArr[i] = false;
     }
   }
  time++;
}

int del_fQ::push(fP pointerF){ // push element from the queue
  if ((last+1)%length == first){
    return 1;
  }
  fQueue[last++] = pointerF;
  last = last%length;
  return 0;
};

int del_fQ::pull(void){ // pull element from the queue
  if (last != first){
  fQueue[first++]();
  first = first%length;
  return 0;
  }
  else{
   return 1;
  }
};
#endif
