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

using namespace std::chrono;

// Timer class
class Timer {
 public:
  Timer();
  virtual ~Timer(){};

  // Start time
  void Start();

  // Stop
  void Stop();

  // Is running
  bool IsRunning() const;

  // Get time running
  template <typename Precision>
  double GetElapsedTime() {
    // Get time now
    if (running) {
      t2 = high_resolution_clock::now();
    }

    // Choose precision
    auto time_diff = duration_cast<Precision>(t2 - t1);

    // Return time as double
    return time_diff.count();
  }

 private:
  // Holds time points for start and end
  time_point<high_resolution_clock> t1, t2;
  bool running;
};

#endif  // TIMER_H
