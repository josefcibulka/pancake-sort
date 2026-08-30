#include "Stack.h"

using std::size_t;
using std::vector;

uint64_t Stack::flip_cnt = 0;

Stack Stack::flip(size_t prefix_size) const {
  Stack result{size()};
  std::copy(&_where_is[0], &_where_is[_len + 1], &(result._where_is[0]));
  for (size_t i = 0; i < prefix_size; ++i) {
    result._val[i] = _val[prefix_size - i - 1];
    result._where_is[result._val[i]] = i;
  }
  std::copy(&_val[prefix_size], &_val[_len + 1], &(result._val[prefix_size]));
  flip_cnt++;
  return result;
}

Stack Stack::extend() const {
  Stack result = *this;
  result._len++;
  result._val[result._len] = result._len;
  result._where_is[result._len] = result._len;
  return result;
}

std::ostream &operator<<(std::ostream &out, const Stack &stack) {
  for (size_t i = 0; i < stack.size(); ++i) {
    out << (int)stack._val[i] << " ";
  }
  return out;
}

size_t Stack::count_adj() const {
  size_t res = 0;
  for (size_t i = 0; i < size(); ++i) {
    res += (abs(_val[i] - _val[i + 1]) == 1);
  }
  return res;
}

bool Stack::is_adj(size_t pos) const {
  return (abs(_val[pos] - _val[pos + 1]) == 1);
}

int Stack::adj_diff(size_t flip_size) const {
  // This works also for flip_size==size(), because we keep _val[size()] =
  // size().
  return (abs(_val[flip_size - 1] - _val[flip_size]) == 1 ? -1 : 0) +
         (abs(_val[0] - _val[flip_size]) == 1 ? 1 : 0);
}

void Stack::bottom_compress() {
  while (size() > 0 && _val[size() - 1] == size() - 1) {
    _len--;
  }
}

uint32_t Stack::perm2index(size_t len, const uint8_t *val) {
  vector<bool> used;
  used.resize(len, false);

  int ret = 0;
  for (size_t i = 0; i < len; i++) {
    int tmp = 0;
    for (uint8_t j = 0; j < val[i]; j++) {
      if (!used[j]) {
        tmp++;
      }
    }
    ret = ret * (len - i) + tmp;
    used[val[i]] = true;
  }

  return ret;
  
}

uint32_t Stack::get_index() const{
  return perm2index(_len, &(_val[0]));
}

