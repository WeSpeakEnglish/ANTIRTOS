#ifndef _NORTOS_H
#define _NORTOS_H

typedef void(*fP)(void);


/// @brief class for finctional pointers queue with supporting of transfer of parameters

class fQ {
private:
    int first;
    int last;
    int length;
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


template <typename T>
class fQP {
private:
    int first;
    int last;
    int length;
    typedef void (*FunctionPointer)(T);
    FunctionPointer* FP_Queue;
    T* PARAMS_array;

public:
    fQP(int sizeQ);
    ~fQP();
    int push(void (*pointerQ)(T), T parameterQ);
    int pull();
};

template <typename T>
fQP<T>::fQP(int sizeQ) {
    FP_Queue = new FunctionPointer[sizeQ];
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
    FunctionPointer pullVar;
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