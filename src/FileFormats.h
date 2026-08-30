#pragma once

#include <fstream>
#include <vector>

#include "BurntStack.h"
#include "GreedoTools.h"

// Writes sets of burnt stacks in a packed binary form. This is used for
// greedos. With each call of write, writes a set of pancakes of increasing
// sizes, first call to write writes stacks of size start, next one of size
// start + 1, ... the last one of size end. All these are written to the same
// file, which is closed with the destruction of the class.
class GreedosWrite {
public:
  void init(int start, int end);
  void write(const std::vector<BurntStack> &greedos);

private:
  std::ofstream _file;
};

// Reads sets of greedos from a file written by GreedosWrite.
class GreedosRead {
public:
  void init(int start, int end);
  GreedoSet read(int n);

private:
  std::ifstream _file;
};
