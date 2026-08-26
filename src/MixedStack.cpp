#include "MixedStack.h"

#include <cinttypes>

using std::size_t;

void MixedStack::compress() {
  bool keep_val[kMaxN];
  bool keep_pos[kMaxN];
  bool new_bur[kMaxN];
  bool new_ori[kMaxN];

  while (_len > 0 && _val[_len - 1] == _len - 1) {
    _len--;
  }

  keep_pos[0] = true;
  keep_val[_val[0]] = true;
  // 0 - no block, 1 increasing block, -1 decreasing block
  int prev_diff = 0;
  for (size_t i = 1; i < size(); ++i) {
    bool keep = true;
    if (prev_diff >= 0 && _val[i] == _val[i - 1] + 1) {
      keep = false;
      new_bur[i - 1] = true;
      new_ori[i - 1] = false;
      prev_diff = 1;
    } else if (prev_diff <= 0 && _val[i] == _val[i - 1] - 1) {
      keep = false;
      new_bur[i - 1] = true;
      new_ori[i - 1] = true;
      prev_diff = -1;
    } else {
      new_bur[i - 1] = false;
      new_ori[i - 1] = false;
      prev_diff = 0;
    }
    keep_pos[i] = keep;
    keep_val[_val[i]] = keep;
  }
  new_bur[size() - 1] = false;
  new_ori[size() - 1] = false;
  // compress positions
  int new_len = 0;
  for (size_t op = 0; op < size(); op++) {
    if (keep_pos[op]) {
      _val[new_len] = _val[op];
      new_bur[new_len] = new_bur[op];
      new_ori[new_len] = new_ori[op];
      new_len++;
    }
  }
  size_t old_len = size();
  _len = new_len;
  size_t val_map[kMaxN];
  // compress values
  for (size_t ov = 0, nv = 0; ov < old_len; ov++) {
    val_map[ov] = nv;
    if (keep_val[ov]) {
      nv++;
    }
  }
  _bur = _ori = 0;
  for (size_t i = 0; i < size(); i++) {
    _val[i] = val_map[_val[i]];
    _bur |= (new_bur[i] ? 1 : 0) << i;
    _ori |= (new_ori[i] ? 1 : 0) << i;
  }
}

void MixedStack::compress_old() {
  while (true) {
    // Position of the first pancake of the adjacency.
    size_t adj = size();
    for (size_t i = 0; i < size(); ++i) {
      if (is_adj(i)) {
        adj = i;
        break;
      }
    }
    if (adj == size()) {
      break;
    }

    uint32_t new_bur = 0;
    uint32_t new_ori = 0;

    uint8_t val_adj = _val[adj];
    if (adj < size() - 1) {
      val_adj = std::min(val_adj, _val[adj + 1]);
    }
    for (size_t i = 0; i < adj; ++i) {
      if (_val[i] > val_adj) {
        _val[i]--;
      }
      new_bur += (bur(i) << i);
      new_ori += (ori(i) << i);
    }
    new_bur += (1 << adj);
    if (_val[adj] > val_adj) {
      new_ori += (1 << adj);
    }
    _val[adj] = val_adj;
    for (size_t i = adj + 1; i < size() - 1; ++i) {
      uint8_t new_val = _val[i + 1];
      if (new_val > val_adj) {
        new_val--;
      }
      _val[i] = new_val;
      new_bur += (bur(i + 1) << i);
      new_ori += (ori(i + 1) << i);
    }
    _len--;
    _bur = new_bur;
    _ori = new_ori;
  }
}

MixedStack MixedStack::flip(size_t prefix_size) {
  if (prefix_size == size()) {
    size_t new_size = size();
    if (_val[0] == size() - 1 && (!bur(0) || ori(0))) {
      new_size--;
    }
    MixedStack result(new_size);
    for (size_t i = 0; i < new_size; ++i) {
      size_t old_pos = size() - i - 1;
      result._val[i] = _val[old_pos];
      if (bur(old_pos)) {
        result._bur += (1 << i);
        if (!ori(old_pos)) {
          result._ori += (1 << i);
        }
      }
    }
    return result;
  }
  // Is an adjacency created?
  bool new_adj = false;
  bool new_ori = false;
  if (_val[0] == _val[prefix_size] - 1 && (!bur(0) || ori(0)) &&
      (!bur(prefix_size) || !ori(prefix_size))) {
    new_adj = true;
    new_ori = false;
  }
  if (_val[0] == _val[prefix_size] + 1 && (!bur(0) || !ori(0)) &&
      (!bur(prefix_size) || ori(prefix_size))) {
    new_adj = true;
    new_ori = true;
  }
  size_t new_size = size() - (size_t)new_adj;

  if (new_adj) {
    MixedStack result(new_size);
    uint8_t adj_val = std::min(_val[0], _val[prefix_size]);
    for (size_t i = 0; i < prefix_size - 1; ++i) {
      size_t old_pos = prefix_size - i - 1;
      uint8_t new_val = _val[old_pos];
      if (new_val > adj_val) {
        new_val--;
      }
      result._val[i] = new_val;
      if (bur(old_pos)) {
        result._bur += (1 << i);
        if (!ori(old_pos)) {
          result._ori += (1 << i);
        }
      }
    }
    // The merged pancake
    result._val[prefix_size - 1] = adj_val;
    result._bur += (1 << (prefix_size - 1));
    if (new_ori) {
      result._ori += (1 << (prefix_size - 1));
    }
    for (size_t i = prefix_size; i < new_size; ++i) {
      uint8_t new_val = _val[i + 1];
      if (new_val > adj_val) {
        new_val--;
      }
      result._val[i] = new_val;
      if (bur(i + 1)) {
        result._bur += (1 << i);
        if (ori(i + 1)) {
          result._ori += (1 << i);
        }
      }
    }
    return result;
  }

  MixedStack result = *this;
  // Clear bur and ori in the prefix - those will be regenerated.
  result._bur = (result._bur >> prefix_size) << prefix_size;
  result._ori = (result._ori >> prefix_size) << prefix_size;
  for (size_t i = 0; i < prefix_size; ++i) {
    size_t old_pos = prefix_size - i - 1;
    result._val[i] = _val[old_pos];
    if (bur(old_pos)) {
      result._bur += (1 << i);
      if (!ori(old_pos)) {
        result._ori += (1 << i);
      }
    }
  }
  return result;
}

std::ostream &operator<<(std::ostream &out, const MixedStack &stack) {
  for (size_t i = 0; i < stack.size(); ++i) {
    if (stack.bur(i)) {
      out << (stack.ori(i) ? '-' : '_');
    }
    out << (int)stack._val[i] << " ";
  }
  return out;
}

bool MixedStack::flip_makes_adjacency(size_t prefix_size) {
  if (prefix_size == size()) {
    return _val[0] == size() - 1 && (!bur(0) || ori(0));
  }

  size_t pos = prefix_size;
  if (_val[0] == _val[pos] - 1 && (!bur(0) || ori(0)) &&
      (!bur(pos) || !ori(pos))) {
    return true;
  }
  if (_val[0] == _val[pos] + 1 && (!bur(0) || !ori(0)) &&
      (!bur(pos) || ori(pos))) {
    return true;
  }
  return false;
}

bool MixedStack::is_adj(size_t pos) {
  if (pos == size() - 1) {
    return _val[pos] == size() - 1 && (!bur(pos) || !ori(pos));
  }

  if (_val[pos] == _val[pos + 1] - 1 && (!bur(pos) || !ori(pos)) &&
      (!bur(pos + 1) || !ori(pos + 1))) {
    return true;
  }
  if (_val[pos] == _val[pos + 1] + 1 && (!bur(pos) || ori(pos)) &&
      (!bur(pos + 1) || ori(pos + 1))) {
    return true;
  }
  return false;
}