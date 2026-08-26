#include "StackLog.h"

#include <cstdio>
#include <vector>

using std::size_t;
using std::vector;

StackLog::StackLog()
{
}

StackLog::~StackLog()
{
}

void StackLog::init(size_t len, size_t minwr, size_t maxwr) {

}

void StackLog::log(const vector<int8_t> &stack, size_t dst) {
  int vn;
  int *p;
  int i;
  for (i = 0; i < vn; i++) {
    printf("%d ", p[i] + 1);
  }
  printf("\n");
}
