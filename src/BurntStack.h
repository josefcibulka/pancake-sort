#pragma once

#include "constants.h"

#include <cinttypes>
#include <cstddef>
#include <iostream>
#include <vector>

class BurntStack {
public:
  explicit BurntStack(const std::vector<uint8_t> &unburnt, const uint16_t ori)
      : _ori(ori), _len(unburnt.size()) {
    for (std::size_t i = 0; i < _len; ++i) {
      _val[i] = unburnt[i];
    }
    _val[_len] = _len;
  }

  explicit BurntStack(uint8_t size) : _len{size} {}

  ~BurntStack() {}

  // Comparison operator for std::sort.
  bool operator< (const BurntStack &b) const;
  static bool unburnt_equals(const BurntStack &a, const BurntStack &b);
  
  std::size_t size() const { return _len; }

  bool ori(std::size_t i) const { return _ori & (1 << i); }

  uint8_t val(std::size_t i) const { return _val[i]; }

  // Returns the result of a prefix flip.
  BurntStack flip(std::size_t prefix_size);

  // Is there an adjacency between positions pos and pos + 1?
  // The last pancake can also form an adjacency with the bottom of the stack.
  bool is_adj(size_t pos);

  friend std::ostream &operator<<(std::ostream &out, const BurntStack &stack);

private:
  // Bitmap storing a burnt pancake's orientation, 1 means upside down.
  uint16_t _ori = 0;
  uint8_t _len;
  uint8_t _val[MAX_SIZE + 1];
};

std::ostream &operator<<(std::ostream &out, const BurntStack &stack);
