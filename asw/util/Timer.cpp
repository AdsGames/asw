#include "Timer.h"

// Init
Timer::Timer()
    : t1(high_resolution_clock::now()),
      t2(high_resolution_clock::now()),
      running(false) {}

// Start Timer
void Timer::Start() {
  t1 = high_resolution_clock::now();
  t2 = high_resolution_clock::now();
  running = true;
}

// Stop
void Timer::Stop() {
  running = false;
  t2 = high_resolution_clock::now();
}

// Is running
bool Timer::IsRunning() const {
  return running;
}
