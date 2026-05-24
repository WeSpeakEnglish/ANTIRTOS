// under MIT license, Aleksei Tertychnyi

#ifndef antirtos_h
#define antirtos_h

/**
 * @class fQ
 * @brief Queue-based task scheduler for parameterless function pointers.
 *
 * Tasks are executed in FIFO order.
 */
class fQ {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type taking no parameters and returning void.
     */
    typedef void (*fP)(void);

    /**
     * @brief  Constructs a task queue with the given capacity.
     * @param  sizeQ Maximum number of tasks the queue can hold.
     */
    fQ(unsigned char sizeQ);

    /** @brief Destructor. Frees allocated queue memory. */
    ~fQ();

    /** @brief Deleted copy constructor. */
    fQ(const fQ&) = delete;
    /** @brief Deleted copy assignment. */
    fQ& operator=(const fQ&) = delete;
    /** @brief Deleted move constructor. */
    fQ(fQ&&) = delete;
    /** @brief Deleted move assignment. */
    fQ& operator=(fQ&&) = delete;

    /**
     * @brief  Pushes a function pointer into the queue.
     * @param  pointerF The function pointer to enqueue.
     * @return 0 on success; 1 if the queue is full.
     */
    int push(fP pointerF);

    /**
     * @brief  Pulls and executes the next function from the queue.
     * @return 0 on success; 1 if the queue is empty.
     */
    int pull(void);

  private:
    volatile unsigned char first;   ///< Read index.
    volatile unsigned char last;      ///< Write index.
    unsigned char length;           ///< Maximum size of the queue.
    fP* fQueue;                     ///< Ring buffer of function pointers.
};

inline fQ::fQ(unsigned char sizeQ) {
  fQueue = new fP[sizeQ];
  last = 0;
  first = 0;
  length = sizeQ;
}

inline fQ::~fQ() {
  delete[] fQueue;
}

inline int fQ::push(fP pointerF) {
  if ((last + 1) % length == first) {
    return 1;
  }
  fQueue[last] = pointerF;
  last = (last + 1) % length;
  return 0;
}

inline int fQ::pull(void) {
  if (last != first) {
    fQueue[first]();
    first = (first + 1) % length;
    return 0;
  }
  return 1;
}

/**
 * @class fQP
 * @brief Queue-based task scheduler for function pointers with one parameter.
 *
 * @tparam T Parameter type passed to enqueued functions.
 */
template <typename T>
class fQP {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type taking one parameter of type @p T and returning void.
     */
    typedef void (*fP)(T);

    /**
     * @brief  Constructs a task queue with the given capacity.
     * @param  sizeQ Maximum number of tasks the queue can hold.
     */
    fQP(unsigned char sizeQ);

    /** @brief Destructor. Frees allocated queue memory. */
    ~fQP();

    /** @brief Deleted copy constructor. */
    fQP(const fQP&) = delete;
    /** @brief Deleted copy assignment. */
    fQP& operator=(const fQP&) = delete;
    /** @brief Deleted move constructor. */
    fQP(fQP&&) = delete;
    /** @brief Deleted move assignment. */
    fQP& operator=(fQP&&) = delete;

    /**
     * @brief  Pushes a function pointer and its parameter into the queue.
     * @param  pointerF    The function pointer to enqueue.
     * @param  parameterQ  The parameter to pass when invoked.
     * @return 0 on success; 1 if the queue is full.
     */
    int push(void (*pointerF)(T), T parameterQ);

    /**
     * @brief  Pulls and executes the next function with its stored parameter.
     * @return 0 on success; 1 if the queue is empty.
     */
    int pull();

  private:
    volatile unsigned char first;   ///< Read index.
    volatile unsigned char last;    ///< Write index.
    unsigned char length;           ///< Maximum size of the queue.
    fP* FP_Queue;                   ///< Ring buffer of function pointers.
    T* PARAMS_array;                ///< Ring buffer of stored parameters.
};

template <typename T>
inline fQP<T>::fQP(unsigned char sizeQ) {
  FP_Queue = new fP[sizeQ];
  PARAMS_array = new T[sizeQ];
  last = 0;
  first = 0;
  length = sizeQ;
}

template <typename T>
inline fQP<T>::~fQP() {
  delete[] FP_Queue;
  delete[] PARAMS_array;
}

template <typename T>
inline int fQP<T>::push(void (*pointerF)(T), T parameterQ) {
  if ((last + 1) % length == first) return 1;
  FP_Queue[last] = pointerF;
  PARAMS_array[last] = parameterQ;
  last = (last + 1) % length;
  return 0;
}

template <typename T>
inline int fQP<T>::pull() {
  fP pullVar;
  if (last != first) {
    T Params = PARAMS_array[first];
    pullVar = FP_Queue[first];
    first = (first + 1) % length;
    pullVar(Params);
    return 0;
  }
  return 1;
}

/**
 * @class del_fQ_t
 * @brief Delayed task scheduler for parameterless function pointers.
 *
 * @tparam Tm Unsigned integer type for tick counter and delays. Default is @c unsigned long.
 */
template <typename Tm = unsigned long>
class del_fQ_t {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type taking no parameters and returning void.
     */
    typedef void (*fP)(void);

    /**
     * @brief  Constructs a delayed scheduler with the given capacity.
     * @param  sizeQ Maximum number of tasks the queue can hold.
     */
    del_fQ_t(unsigned char sizeQ);

    /** @brief Destructor. Frees allocated queue memory. */
    ~del_fQ_t();

    /** @brief Deleted copy constructor. */
    del_fQ_t(const del_fQ_t&) = delete;
    /** @brief Deleted copy assignment. */
    del_fQ_t& operator=(const del_fQ_t&) = delete;
    /** @brief Deleted move constructor. */
    del_fQ_t(del_fQ_t&&) = delete;
    /** @brief Deleted move assignment. */
    del_fQ_t& operator=(del_fQ_t&&) = delete;

    /**
     * @brief  Schedules a function for delayed execution.
     * @param  pointerF   The function pointer to enqueue.
     * @param  delayTime  Delay in ticks before execution. Zero means immediate.
     * @return 0 on success; 1 if the delayed queue is full.
     *
     * @note Uses a sequence-locked snapshot of the tick counter to avoid torn
     *       reads on 8-bit platforms. For this to be safe, tick() must update
     *       the snapshot atomically (e.g., from an ISR or with interrupts off).
     */
    int push_delayed(fP pointerF, Tm delayTime);

    /**
     * @brief  Enqueues a function for immediate execution.
     * @param  pointerF The function pointer to enqueue.
     * @return 0 on success; 1 if the queue is full.
     */
    int push(fP pointerF);

    /**
     * @brief  Periodic tick handler. Call from a timer ISR or main loop.
     * @warning push_delayed() and tick() must not be called from contexts
     *          that can preempt each other arbitrarily. Specifically,
     *          push_delayed() must run at strictly lower priority than tick(),
     *          or all calls must be serialized at the same priority level.
     *          The internal busy flag protects only the case where push_delayed()
     *          is in progress and tick() preempts it.
     */
    void tick(void);

    /**
     * @brief  Pulls and executes the next immediate task.
     * @return 0 on success; 1 if the queue is empty.
     */
    int pull(void);

    /**
     * @brief  Cancels a pending delayed task.
     * @param  pointerF The function pointer to revoke.
     * @return 0 if found and cancelled; 1 if not found.
     */
    int revoke(fP pointerF);

  private:
    volatile unsigned char first;      ///< Read index of the immediate queue.
    volatile unsigned char last;       ///< Write index of the immediate queue.
    unsigned char length;              ///< Maximum size of the queue.
    volatile Tm time;                  ///< Live tick counter.
    volatile Tm time_snap;             ///< Sequence-locked snapshot of @c time.
    volatile unsigned char time_seq;   ///< Sequence generation counter (single byte, atomic on 8-bit AVR).
    volatile unsigned char busy;       ///< Busy flag protecting the delayed queue from concurrent tick() scan.
    fP*  fQueue;                     ///< Immediate execution ring buffer.
    fP*  del_fQueue;                  ///< Delayed execution function buffer.
    bool* execArr;                   ///< Slot-occupied flags for delayed tasks.
    Tm*  execTime;                   ///< Target tick values for delayed tasks.
};

template <typename Tm>
inline del_fQ_t<Tm>::del_fQ_t(unsigned char sizeQ) {
  fQueue = new fP[sizeQ];
  del_fQueue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  execTime = new Tm[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  time_snap = 0;
  time_seq = 0;
  busy = 0;
  for (unsigned char i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
}

template <typename Tm>
inline del_fQ_t<Tm>::~del_fQ_t() {
  delete[] fQueue;
  delete[] del_fQueue;
  delete[] execArr;
  delete[] execTime;
}

template <typename Tm>
inline int del_fQ_t<Tm>::push_delayed(fP pointerF, Tm delayTime) {
  if (delayTime == 0) {
    return push(pointerF);
  }

  Tm now;
  unsigned char seq1, seq2;
  do {
    seq1 = time_seq;
    now  = time_snap;
    seq2 = time_seq;
  } while (seq1 != seq2);

  busy = 1;
  bool fullQ = true;
  for (unsigned char i = 0; i < length; i++) {
    if (!execArr[i]) {
      del_fQueue[i] = pointerF;
      execArr[i]    = true;
      execTime[i]   = now + delayTime;
      fullQ = false;
      break;
    }
  }
  busy = 0;
  return fullQ ? 1 : 0;
}

template <typename Tm>
inline int del_fQ_t<Tm>::push(fP pointerF) {
  if ((last + 1) % length == first) {
    return 1;
  }
  fQueue[last] = pointerF;
  last = (last + 1) % length;
  return 0;
}

template <typename Tm>
inline void del_fQ_t<Tm>::tick(void) {
  time++;
  time_snap = time;
  ++time_seq;

  if (!busy) {
    for (unsigned char i = 0; i < length; i++) {
      if (execArr[i]) {
        Tm elapsed = static_cast<Tm>(time - execTime[i]);
        const Tm halfRange = static_cast<Tm>(static_cast<Tm>(~static_cast<Tm>(0)) >> 1);
        if (elapsed <= halfRange) {
          push(del_fQueue[i]);
          execArr[i] = false;
        }
      }
    }
  }
}

template <typename Tm>
inline int del_fQ_t<Tm>::pull(void) {
  if (last != first) {
    fQueue[first]();
    first = (first + 1) % length;
    return 0;
  }
  return 1;
}

template <typename Tm>
inline int del_fQ_t<Tm>::revoke(fP pointerF) {
  int result = 1;
  busy = 1;
  for (unsigned char i = 0; i < length; i++) {
    if (execArr[i] && del_fQueue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  busy = 0;
  return result;
}

/**
 * @class del_fQ
 * @brief Convenience alias for @c del_fQ_t with default @c unsigned long time type.
 */
class del_fQ : public del_fQ_t<unsigned long> {
  public:
    /** @brief Constructs a delayed scheduler with the given capacity. */
    del_fQ(unsigned char sizeQ) : del_fQ_t<unsigned long>(sizeQ) {}
};

/**
 * @class del_fQP
 * @brief Delayed task scheduler for function pointers with one parameter.
 *
 * @tparam T  Parameter type passed to enqueued functions.
 * @tparam Tm Unsigned integer type for tick counter and delays. Default is @c unsigned long.
 */
template <typename T, typename Tm = unsigned long>
class del_fQP {
  public:
    /**
     * @typedef fP
     * @brief Function pointer type taking one parameter of type @p T and returning void.
     */
    typedef void (*fP)(T);

    /**
     * @brief  Constructs a delayed scheduler with the given capacity.
     * @param  sizeQ Maximum number of tasks the queue can hold.
     */
    del_fQP(unsigned char sizeQ);

    /** @brief Destructor. Frees allocated queue memory. */
    ~del_fQP();

    /** @brief Deleted copy constructor. */
    del_fQP(const del_fQP&) = delete;
    /** @brief Deleted copy assignment. */
    del_fQP& operator=(const del_fQP&) = delete;
    /** @brief Deleted move constructor. */
    del_fQP(del_fQP&&) = delete;
    /** @brief Deleted move assignment. */
    del_fQP& operator=(del_fQP&&) = delete;

    /**
     * @brief  Enqueues a function and parameter for immediate execution.
     */
    int push(void (*pointerF)(T), T parameterQ);

    /**
     * @brief  Schedules a function and parameter for delayed execution.
     */
    int push_delayed(void (*pointerF)(T), T parameterQ, Tm delayTime);

    /**
     * @brief  Periodic tick handler. Call from a timer ISR or main loop.
     * @warning push_delayed() and tick() must not be called from contexts
     *          that can preempt each other arbitrarily. Specifically,
     *          push_delayed() must run at strictly lower priority than tick(),
     *          or all calls must be serialized at the same priority level.
     *          The internal busy flag protects only the case where push_delayed()
     *          is in progress and tick() preempts it.
     */
    void tick(void);

    /**
     * @brief  Cancels a pending delayed task.
     */
    int revoke(void (*pointerF)(T));

    /**
     * @brief  Pulls and executes the next immediate task with its stored parameter.
     */
    int pull();

  private:
    volatile unsigned char first;              ///< Read index of the immediate queue.
    volatile unsigned char last;               ///< Write index of the immediate queue.
    unsigned char length;                      ///< Maximum size of the queue.
    volatile Tm time;                          ///< Live tick counter.
    volatile Tm time_snap;                     ///< Sequence-locked snapshot of @c time.
    volatile unsigned char time_seq;           ///< Sequence generation counter (single byte, atomic on 8-bit AVR).
    volatile unsigned char busy;               ///< Busy flag protecting the delayed queue from concurrent tick() scan.
    fP*  FP_Queue;                             ///< Immediate execution ring buffer.
    fP*  del_FP_Queue;                         ///< Delayed execution function buffer.
    bool* execArr;                             ///< Slot-occupied flags for delayed tasks.
    Tm*  execTime;                             ///< Target tick values for delayed tasks.
    T*   PARAMS_array;                         ///< Stored parameters for immediate tasks.
    T*   delayed_PARAMS_array;                 ///< Stored parameters for delayed tasks.
};

template <typename T, typename Tm>
inline del_fQP<T, Tm>::del_fQP(unsigned char sizeQ) {
  FP_Queue = new fP[sizeQ];
  del_FP_Queue = new fP[sizeQ];
  execArr = new bool[sizeQ];
  PARAMS_array = new T[sizeQ];
  delayed_PARAMS_array = new T[sizeQ];
  execTime = new Tm[sizeQ];
  last = 0;
  first = 0;
  time = 0;
  time_snap = 0;
  time_seq = 0;
  busy = 0;
  for (unsigned char i = 0; i < sizeQ; i++) {
    execArr[i] = false;
  }
  length = sizeQ;
}

template <typename T, typename Tm>
inline del_fQP<T, Tm>::~del_fQP() {
  delete[] FP_Queue;
  delete[] del_FP_Queue;
  delete[] PARAMS_array;
  delete[] delayed_PARAMS_array;
  delete[] execArr;
  delete[] execTime;
}

template <typename T, typename Tm>
inline int del_fQP<T, Tm>::push(void (*pointerF)(T), T parameterQ) {
  if ((last + 1) % length == first) return 1;
  FP_Queue[last] = pointerF;
  PARAMS_array[last] = parameterQ;
  last = (last + 1) % length;
  return 0;
}

template <typename T, typename Tm>
inline int del_fQP<T, Tm>::push_delayed(void (*pointerF)(T), T parameterQ, Tm delayTime) {
  if (delayTime == 0) {
    return push(pointerF, parameterQ);
  }

  Tm now;
  unsigned char seq1, seq2;
  do {
    seq1 = time_seq;
    now  = time_snap;
    seq2 = time_seq;
  } while (seq1 != seq2);

  busy = 1;
  bool fullQ = true;
  for (unsigned char i = 0; i < length; i++) {
    if (!execArr[i]) {
      del_FP_Queue[i]      = pointerF;
      delayed_PARAMS_array[i] = parameterQ;
      execArr[i]           = true;
      execTime[i]          = now + delayTime;
      fullQ = false;
      break;
    }
  }
  busy = 0;
  return fullQ ? 1 : 0;
}

template <typename T, typename Tm>
inline void del_fQP<T, Tm>::tick(void) {
  time++;
  time_snap = time;
  ++time_seq;

  if (!busy) {
    for (unsigned char i = 0; i < length; i++) {
      if (execArr[i]) {
        Tm elapsed = static_cast<Tm>(time - execTime[i]);
        const Tm halfRange = static_cast<Tm>(static_cast<Tm>(~static_cast<Tm>(0)) >> 1);
        if (elapsed <= halfRange) {
          push(del_FP_Queue[i], delayed_PARAMS_array[i]);
          execArr[i] = false;
        }
      }
    }
  }
}

template <typename T, typename Tm>
inline int del_fQP<T, Tm>::revoke(void (*pointerF)(T)) {
  int result = 1;
  busy = 1;
  for (unsigned char i = 0; i < length; i++) {
    if (execArr[i] && del_FP_Queue[i] == pointerF) {
      execArr[i] = false;
      result = 0;
    }
  }
  busy = 0;
  return result;
}

template <typename T, typename Tm>
inline int del_fQP<T, Tm>::pull() {
  fP pullVar;
  if (last != first) {
    T Params = PARAMS_array[first];
    pullVar = FP_Queue[first];
    first = (first + 1) % length;
    pullVar(Params);
    return 0;
  }
  return 1;
}

#endif
