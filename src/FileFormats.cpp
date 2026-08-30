#include "FileFormats.h"

#include <format>

#include "Stack.h"

using std::string;
using std::vector;

void GreedosWrite::init(int start, int end) {
  string filename = std::format("greedos{}-{}.bing", start, end);
  _file.open(filename, std::ios::binary);
}

void GreedosWrite::write(const vector<BurntStack> &greedos) {
  size_t i = 0;
  uint32_t cur_ind = greedos[i].to_unburnt().get_index();
  for (uint32_t j = 0; j < cur_ind; ++j) {
    char val = 0;
    _file.write(&val, 1);
  }
  // First write, for each unburnt stack, how many burnt variants of it are
  // greedos.
  while (i < greedos.size()) {
    size_t ni = i + 1;
    while (ni < greedos.size() &&
           cur_ind == greedos[ni].to_unburnt().get_index()) {
      ++ni;
    }
    char val = ni - i;
    _file.write(&val, 1);
    uint32_t next_ind = ni < greedos.size()
                            ? greedos[ni].to_unburnt().get_index()
                            : greedos.size();
    for (uint32_t j = cur_ind + 1; j < next_ind; ++j) {
      char val = 0;
      _file.write(&val, 1);
    }
    i = ni;
    cur_ind = next_ind;
  }
  // Then, for each greedo, write its panacakes' orientations in 2 bytes.
  for (const BurntStack &greedo : greedos) {
    uint16_t ori = greedo.ori_bitmap();
    _file.write(reinterpret_cast<char *>(&ori), 2);
  }
}

void GreedosRead::init(int start, int end) {
  string filename = std::format("greedos{}-{}.bing", start, end);
  _file.open(filename, std::ios::binary);
}

GreedoSet GreedosRead::read(int n) {
  GreedoSet res;
  char buffer[2];

  // Size is n-factorial.
  uint32_t size = 1;
  for (int i = 2; i <= n; ++i) {
    size *= i;
  }

  for (uint32_t i = 0; i < size; ++i) {
    _file.read(buffer, 1);
    if (!_file) {
      std::cerr << "Failed to read from file!" << std::endl;
      exit(1);
    }
    res.add_count(buffer[0]);
  }

  for (uint32_t i = 0; i < size; ++i) {
    _file.read(buffer, 2);
    if (!_file) {
      std::cerr << "Failed to read from file!" << std::endl;
      exit(1);
    }
    res.add_ori(reinterpret_cast<uint16_t *>(&buffer)[0]);
  }
  return res;
}
