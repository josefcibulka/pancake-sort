#pragma once

#include <cinttypes>
#include <vector>

class StackLog
{
public:
  StackLog();
  ~StackLog();

  // Creates the output files.
  void init(std::size_t len, std::size_t minwr, std::size_t maxwr);
  void finalize();
  
  void log(const std::vector<int8_t> &stack, std::size_t dst);
};

