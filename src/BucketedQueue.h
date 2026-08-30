#pragma once

#include <cctype>
#include <cinttypes>
#include <iostream>

#include <array>
#include <functional>
#include <queue>

#include "Stack.h"
#include "constants.h"

class HeapElement {
public:
  enum FlipsProcessed {
    FLIPS_NONE = 0,
    FLIPS_JOINS = 1, // joins were tried and the resulting stacks are in heap
    FLIPS_NON_NEGATIVE = 2, // flips not decreasing number of adjacencies tried
  };

  HeapElement(
      Stack s_, uint8_t dep_, uint8_t lb_, uint8_t racnt_,
      HeapElement::FlipsProcessed flips_processed_ = HeapElement::FLIPS_NONE,
      bool known_wastie_ = false)
      : s(s_), dep(dep_), lb(lb_), racnt(racnt_),
        flips_processed(flips_processed_), known_wastie(known_wastie_) {}

  Stack s;
  uint8_t dep;   // depth ... distance (number of flips) from the initial stack
  uint8_t lb;    // lower bound on the initial stack (i.e., dep is counted, too)
  uint8_t racnt; // remaining adjacencies
  // Types of flips that were tried and the resulting stacks were added to the
  // heap. The lower bound lb already takes this into account, so when
  // flips_processed are joins, the lb is the number of adjacencies remaining to
  // be done plus one.
  FlipsProcessed flips_processed;
  bool known_wastie;
};

// A 2-level queue used in the A* search.
// First level: buckets per lower bound, the lower ones are taken first
// Second, leaf level: buckets per depth, the higher ones are taken first
//                   - the reason is that they are closer to the target
class BucketedQueue {
public:
  static uint64_t maxheapsize;

  BucketedQueue();
  ~BucketedQueue() {}

  // Pops the next element (smallest lower bound, highest depth) and returns it.
  HeapElement top_pop();

  void add(HeapElement el);

  // Resets the bucketed queue. This is preferred over recreating it, because
  // when resizing, the compiler keeps the allocation of the vector as is, so
  // we don't lose time on re-allocations when the vector grows again.
  void reset();

  bool empty() { return _size == 0; }

  // Whether a fist-level bucket is empty.
  bool empty_lb(std::size_t lb) { return _size_lb[lb] == 0; }

  size_t size() { return _size; }

private:
  // Each vector is one leaf bucket
  std::vector<HeapElement> _qs[MAX_STEP][MAX_STEP];
  // The sum of the number of element in the buckets.
  std::size_t _size = 0;
  // The smallest lower bound with a non-empty bucket.
  int _first_lb = MAX_STEP;
  // Size of the first-level buckets.
  std::size_t _size_lb[MAX_STEP]{};
  // For each lower bound lb, a bitmap specifying for which depths dep,
  // _qs[lb][dep] is non-empty. The leftmost 1-entry then determines from which
  // _qs to take the next stack for processing.
  unsigned _dep_bitmap[MAX_STEP];
};
