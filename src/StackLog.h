#pragma once

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <vector>

#include "Stack.h"

class StackLog {
public:
  StackLog();

  // No need to close the files, the destructor of ofstream does it.
  ~StackLog();

  // Creates the output files containing the stacks.
  bool init(std::size_t len, std::size_t minwr, std::size_t maxwr, int frac,
            int modeq);

  void log_stack(const Stack &s, std::size_t dst);

  void write_stack_counts(int start);
  void write_stats(int triescnt, int flip_cnt, int maxheapsize);

private:
  std::size_t _len;
  std::size_t _minwr;
  std::size_t _maxwr;
  int _frac;
  int _modeq;

  uint64_t _visstackcnt[MAX_SIZE + 2][MAX_STEP + 2]; // data for .scnt file
  std::size_t _diam[MAX_SIZE + 2]; // diam[i] = maximum number of flips among
                                   // tried stacks of size i

  std::vector<std::ofstream> _outf;
};
