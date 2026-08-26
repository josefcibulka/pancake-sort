#include "BucketedQueue.h"

#include <bit>
#include <cassert>


uint64_t BucketedQueue::maxheapsize = 0;

BucketedQueue::BucketedQueue() {}

void BucketedQueue::reset() {
  _size = 0;
  _first_lb = MAX_STEP;
  for (size_t i = 0; i < MAX_STEP; ++i) {
    _size_lb[i] = 0;
    _dep_bitmap[i] = 0;
    for (size_t j = 0; j < MAX_STEP; ++j) {
      _qs[i][j].clear();
    }
  }
}
HeapElement BucketedQueue::top_pop() {
  assert(!empty());
  int fl = _first_lb;
  // This first-level bucket becomes empty after the pop.
  if (_size_lb[_first_lb] == 1) {
    for (std::size_t i = fl + 1; i < MAX_STEP; ++i) {
      if (!empty_lb(i)) {
        _first_lb = i;
        break;
      }
    }
    if (_first_lb == fl) {
      _first_lb = MAX_STEP;
    }
  }

  int ld = 31 - std::countl_zero(_dep_bitmap[fl]);  
  // This leaf bucket becomes empty after the pop.
  if (_qs[fl][ld].size() == 1) {
    _dep_bitmap[fl] -= (1 << ld);
  }

  HeapElement el = _qs[fl][ld].back();
  _qs[fl][ld].pop_back();
  _size--;
  _size_lb[fl]--;
  return el;
}

void BucketedQueue::add(HeapElement el) {
#ifdef DEBUG_PRINT
  std::cout << "Adding to heap: " << el.s << "racnt: " << el.racnt
            << " lb: " << el.lb << " dep : " << el.dep << std::endl;
#endif

  if (_qs[el.lb][el.dep].empty()) {
    if (el.lb < _first_lb) {
      _first_lb = el.lb;
    }
    _dep_bitmap[el.lb] += (1 << el.dep);
  }
  _qs[el.lb][el.dep].push_back(el);
  _size++;
  maxheapsize = std::max(_size, maxheapsize);
  _size_lb[el.lb]++;
}
