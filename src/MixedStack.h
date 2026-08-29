#pragma once

#include <cinttypes>
#include <cstddef>
#include <iostream>
#include <vector>

#include "BurntStack.h"
#include "Stack.h"

class MixedStack {
public:
  static constexpr size_t kMaxN = 20;

  explicit MixedStack(const std::vector<uint8_t> &unburnt)
      : _len(unburnt.size()), _orig(unburnt) {
    for (std::size_t i = 0; i < _len; ++i) {
      _val[i] = unburnt[i];
    }
    // compress();
  }

  explicit MixedStack(const Stack &unburnt) : _len(unburnt.size()) {
    for (std::size_t i = 0; i < _len; ++i) {
      _val[i] = unburnt.val(i);
    }
    // compress();
  }

  explicit MixedStack(std::size_t size) : _len{size} {}

  ~MixedStack() {}

  std::size_t size() const { return _len; }

  bool bur(std::size_t i) const { return _bur & (1 << i); }

  bool ori(std::size_t i) const { return _ori & (1 << i); }

  uint8_t val(std::size_t i) const { return _val[i]; }

  const std::vector<uint8_t> &orig() const { return _orig; }

  // Returns the result of a prefix flip. If an adjacency appears, compress it.
  MixedStack flip(std::size_t prefix_size);

  // Returns true if the prefix_size flip will create an adjacency.
  bool flip_makes_adjacency(std::size_t prefix_size);

  friend std::ostream &operator<<(std::ostream &out, const MixedStack &stack);

  // Compresses the stack by merging adjacencies. The stack must be unburnt.
  void compress();

  void compress_old();

  // For a BurntStack decides if orientations, where present, are the same.
  bool is_ori_compatible(const BurntStack &bs) const;

private:
  std::size_t _len;
  // Bitmap storing whether a pancake is burnt.
  uint32_t _bur = 0;
  // Bitmap storing whether a burnt pancake's orientation, 1 means upside down.
  uint32_t _ori = 0;
  uint8_t _val[30];
  // The original unburnt stack. Only set by the constructor that takes an
  // unburnt stack.
  std::vector<uint8_t> _orig;

  // Is there an adjacency between positions pos and pos + 1?
  // The last pancake can also form an adjacency with the bottom of the stack.
  bool is_adj(size_t pos);
};

std::ostream &operator<<(std::ostream &out, const MixedStack &stack);
