#include "BurntStack.h"

using std::size_t;

bool BurntStack::operator<(const BurntStack &b) const {
  if (_len != b._len) {
    return _len < b._len;
  }
  for (std::size_t i = 0; i < _len; ++i) {
    if (_val[i] != b._val[i]) {
      return _val[i] < b._val[i];
    }
  }
  return _ori < b._ori;
}

bool BurntStack::unburnt_equals(const BurntStack &a, const BurntStack &b) {
  if (a._len != b._len) {
    return false;
  }
  for (std::size_t i = 0; i < a._len; ++i) {
    if (a._val[i] != b._val[i]) {
      return false;
    }
  }
  return true;
}

BurntStack BurntStack::flip(size_t prefix_size) {
  BurntStack result = *this;
  // Clear ori in the prefix - those will be regenerated.
  result._ori = (result._ori >> prefix_size) << prefix_size;
  for (size_t i = 0; i < prefix_size; ++i) {
    size_t old_pos = prefix_size - i - 1;
    result._val[i] = _val[old_pos];
    if (!ori(old_pos)) {
      result._ori += (1 << i);
    }
  }
  return result;
}

std::ostream &operator<<(std::ostream &out, const BurntStack &stack) {
  for (size_t i = 0; i < stack.size(); ++i) {
    out << (stack.ori(i) ? '-' : '_');
    out << (int)stack._val[i] << " ";
  }
  return out;
}

bool BurntStack::is_adj(size_t pos) {
  if (_val[pos] == _val[pos + 1] - 1 && !ori(pos) && !ori(pos + 1)) {
    return true;
  }
  if (_val[pos] == _val[pos + 1] + 1 && ori(pos) && ori(pos + 1)) {
    return true;
  }
  return false;
}