#include "./asw/util/Timer.h"

// Start Timer
void Timer::start() {
  t1 = std::chrono::high_resolution_clock::now();
  t2 = std::chrono::high_resolution_clock::now();
  running = true;
}

// Stop
void Timer::stop() {
  running = false;
  t2 = std::chrono::high_resolution_clock::now();
}

// Is running
bool Timer::isRunning() const {
  return running;
}

// Reset
void Timer::reset() {
  t1 = std::chrono::high_resolution_clock::now();
  t2 = std::chrono::high_resolution_clock::now();
}