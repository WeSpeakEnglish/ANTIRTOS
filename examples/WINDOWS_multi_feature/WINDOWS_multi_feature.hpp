
#ifndef win32_static_helper_h
#define win32_static_helper_h

#include <atomic>
#include <functional>
#include <thread>

class Timer
{
public:
  Timer() : running(false)
  {
  }

  void start(int interval_ms, std::function<void()> callback)
  {
    running = true;
    timerThread = std::thread(
        [=]()
        {
          while (running)
          {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            if (running)
            { // Double-check to prevent callback on stop.
              callback();
            }
          }
        });
  }

  void stop()
  {
    running = false;
    if (timerThread.joinable())
    {
      timerThread.join();
    }
  }

  ~Timer()
  {
    stop(); // Ensure the timer stops on destruction
  }

private:
  std::atomic<bool> running;
  std::thread timerThread;
};

#endif // win32_static_helper_h
