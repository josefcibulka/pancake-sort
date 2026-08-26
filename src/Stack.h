#pragma once

#include <cinttypes>
#include <cstddef>
#include <iostream>
#include <vector>

#include "constants.h"

class Stack {
public:
  static uint64_t flip_cnt;

  explicit Stack(const std::vector<uint8_t> &unburnt) : _len{unburnt.size()} {
    for (std::size_t i = 0; i < _len; ++i) {
      _val[i] = unburnt[i];
      _where_is[unburnt[i]] = i;
    }
    _val[_len] = _len;
    _where_is[_len] = _len;
  }

  explicit Stack(std::size_t size) : _len{size} {}

  ~Stack() {}

  std::size_t size() const { return _len; }

  uint8_t val(size_t i) const { return _val[i]; }

  // Returns the result of a prefix flip.
  Stack flip(std::size_t prefix_size) const;

  // Add a new largest element to the end of the stack.
  Stack extend() const;

  size_t count_adj() const;

  // Is there an adjacency between positions pos and pos + 1?
  // The last pancake can also form an adjacency with the bottom of the stack.
  bool is_adj(size_t pos) const;

  // What will be the difference in number of adjacencies when we flip the top
  // flip_size pancakes?
  int adj_diff(size_t flip_size) const;

  void bottom_compress();

  friend std::ostream &operator<<(std::ostream &out, const Stack &stack);

  uint8_t _val[MAX_SIZE + 1];
  uint8_t _where_is[MAX_SIZE + 1];

  size_t _len;
};

std::ostream &operator<<(std::ostream &out, const Stack &stack);
