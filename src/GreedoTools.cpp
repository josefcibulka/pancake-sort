#include "GreedoTools.h"

#include <iostream>

uint64_t GreedoSet::check_cnt = 0;

void GreedoSet::add_count(uint8_t val) {
  size_t old_size = _counts.size();
  _counts.push_back(val);
  if ((old_size & (sparsity - 1)) == 0) {
    if (old_size == 0) {
      _sparse_ids.push_back(0);
    } else {
      uint32_t new_id = *(_sparse_ids.rbegin());
      for (size_t i = old_size - sparsity; i < old_size; ++i) {
        new_id += _counts[i];
      }
      _sparse_ids.push_back(new_id);
    }
  }
}

void GreedoSet::add_ori(uint16_t ori) { _oris.push_back(ori); }

bool GreedoSet::check_stack(const MixedStack &s) {
  check_cnt++;
  uint32_t index = s.get_index();
  if(_counts[index] == 0) {
    return false;
  }
  uint32_t sparse = (index >> sparsity_log);
  uint32_t ori_start = _sparse_ids[sparse];
  for (size_t i = (sparse << sparsity_log); i < index; ++i) {
    ori_start += _counts[i];
  }
  for (size_t i = ori_start; i < ori_start + _counts[index]; ++i) {
    if (s.is_ori_compatible(_oris[i])) {
      return true;
    }
  }
  return false;
}