#include "StackLog.h"

#include <algorithm>
#include <cstdio>
#include <format>
#include <ios>
#include <string>
#include <vector>

#ifdef unix
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#else
#include "psapi.h"
#include "windows.h"
#endif

using std::format;
using std::size_t;
using std::string;
using std::vector;

namespace {
double my_get_seconds(double *user_time, double *system_time) {
  double ut, st;
#ifdef unix
  struct tms buffer;
  times(&buffer);
  ut = buffer.tms_utime / (double)sysconf(_SC_CLK_TCK);
  st = buffer.tms_stime / (double)sysconf(_SC_CLK_TCK);
#else
  ULARGE_INTEGER nanohun;
  FILETIME CreationTime, ExitTime, KernelTime, UserTime;

  GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime,
                  &UserTime);
  memcpy(&nanohun, &UserTime, sizeof(ULARGE_INTEGER));
  ut = ((double)(nanohun.QuadPart)) / 10000000.0;
  memcpy(&nanohun, &KernelTime, sizeof(ULARGE_INTEGER));
  st = ((double)(nanohun.QuadPart)) / 10000000.0;
#endif
  if (user_time)
    *user_time = ut;
  if (system_time)
    *system_time = st;
  return ut + st;
}
}; // namespace

StackLog::StackLog() {}

StackLog::~StackLog() {}

bool StackLog::init(size_t len, size_t minwr, size_t maxwr, int frac,
                    int modeq) {
  _len = len;
  _minwr = minwr;
  _maxwr = maxwr;
  _frac = frac;
  _modeq = modeq;
  _outf.clear();
  for (size_t i = minwr; i <= maxwr; i++) {
    string filename;
    if (frac == 1) {
      filename = format("{}-{}.in", len, i);
    } else {
      filename = format("{}-{}-{:0>4}of{}.in", len, i, modeq, frac);
    }
    {
      // Confirm the file does not exist.
      std::ifstream ifs;
      ifs.open(filename);
      if (!ifs.fail()) {
        std::cerr << "Output file " << filename << " already exists"
                  << std::endl;
        return false;
      }
      ifs.close();
    }
    std::ofstream ofs;
    ofs.open(filename);
    _outf.push_back(std::move(ofs));
  }
  return true;
}

void StackLog::log_stack(const Stack &s, size_t dst) {
  _visstackcnt[s.size()][dst]++;
  _diam[s.size()] = std::max(_diam[s.size()], dst);
  if (s.size() == _len && dst >= _minwr && dst <= _maxwr) {
    for (std::size_t i = 0; i < s.size(); ++i) {
      _outf[dst - _minwr] << (s.val(i) + 1) << " ";
    }
    _outf[dst - _minwr] << std::endl;
    return;
  }
}

void StackLog::write_stack_counts(int start) {
  string filename;
  if (_frac == 1) {
    filename = format("{}-{}-{}.scnt", _len, _minwr, _maxwr);
  } else {
    filename =
        format("{}-{}-{}-{:0>4}of{}.scnt", _len, _minwr, _maxwr, _modeq, _frac);
  }
  std::ofstream ofs;
  ofs.open(filename);
  ofs << format("manysteps {} {} {} {} {:4}", _len, _minwr, _maxwr, _frac,
                _modeq)
      << std::endl
      << "Stack counts:" << std::endl;
  for (size_t i = start; i <= _len; i++) {
    ofs << format("n=={:0>2}, diam=={:0>2} ", i, _diam[i]);
    for (int j = _maxwr; j >= 0 && j + 2 * (_len - i) >= _minwr; j--)
      if (_visstackcnt[i][j] != 0)
        ofs << format("{:8} ", _visstackcnt[i][j]);
    ofs << std::endl;
  }
}

void StackLog::write_stats(int triescnt, int flip_cnt, int maxheapsize) {
  string filename;
  if (_frac == 1) {
    filename = format("{}-{}-{}.time", _len, _minwr, _maxwr);
  } else {
    filename =
        format("{}-{}-{}-{:0>4}of{}.time", _len, _minwr, _maxwr, _modeq, _frac);
  }
  std::ofstream ofs;
  ofs.open(filename);

  double time = my_get_seconds(nullptr, nullptr);
  ofs << format("manysteps {} {} {} {} {:4} stacks tried: {:10}  flips done: "
                "{:16} max heap size {:12} {:10.3f}",
                _len, _minwr, _maxwr, _frac, _modeq, triescnt, flip_cnt,
                maxheapsize, time)
      << std::endl;
}
