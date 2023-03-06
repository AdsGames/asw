/*
 * Timer.h
 * A simple, cross platform timer class
 *  based on chrono
 * Allan Legemaate
 * 25-03-2019
 */

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

// Timer class
class Timer {
 public:
  // Start time
  void start();

  // Stop
  void stop();

  // Is running
  bool isRunning() const;

  // Reset timer
  void reset();

  // Get time running
  template <typename Precision>
  double getElapsedTime() {
    // Get time now
    if (running) {
      t2 = std::chrono::high_resolution_clock::now();
    }

    // Choose precision
    auto time_diff = std::chrono::duration_cast<Precision>(t2 - t1);

    // Return time as double
    return time_diff.count();
  }

 private:
  // Holds time points for start and end
  std::chrono::time_point<std::chrono::high_resolution_clock> t1{
      std::chrono::high_resolution_clock::now()};
  std::chrono::time_point<std::chrono::high_resolution_clock> t2{
      std::chrono::high_resolution_clock::now()};
  bool running{false};
};

#endif  // TIMER_H
