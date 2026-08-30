#pragma once

#include <cstdint>
#include <vector>

#include "MixedStack.h"

class GreedoSet {
public:
  static uint64_t check_cnt;
  void add_count (uint8_t val); 
  void add_ori(uint16_t ori);
  
  bool check_stack(const MixedStack &s);

private:
  static constexpr int sparsity_log{3};
  static constexpr int sparsity{8};

  // For every unburnt stack, how many greedos are orientations of this stack.
  std::vector<uint8_t> _counts;
  // For every sparsity-th unburnt stack, contains the index of its first greedo
  // in the _oris. Together with _counts allows getting the index of the first
  // greedo of every unburnt stack.
  std::vector<uint32_t> _sparse_ids;
  std::vector<uint16_t> _oris;
};
