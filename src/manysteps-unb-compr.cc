/*
    Pancake sorting program (see kam.mff.cuni.cz/~cibulka/pancakes)
    Copyright (C) 2026  Josef Cibulka

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* See e.g. wikipedia for the definition of pancake sorting.
 * See http://arxiv.org/abs/0901.3119 for more details on the algorithm
 * including references to the sources of its main ideas.
 *
 * Finds all unburnt stacks of a given size globn with required number of flips
 * from a given interval {minwr, ..., maxwr}.
 * Starts by generating the stacks of size START. From each of them, generates
 * stacks of size globn in steps. In each step, a stack is enlarged by one by
 * first adding a new largest pancake to the bottom (this stack needs exactly
 * the same number of flips as the original one) and then doing two flips, first
 * of which flips the whole stack. This way all the stacks of size globn would
 * be generated. The intermediate stacks that would produce stacks of size globn
 * sortable in < minwr flips are pruned. A*search is used to determine the
 * needed number of flips for a given stack.
 *
 * User can also choose to generate only 1/frac of stacks of size START.
 * Run the program without paramaters to find out how.
 *
 * Memory consumption is at most MAXHEAPELEM*(MAXN+12) bytes.
 */

#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <functional>
#include <queue>
#include <unordered_set>
#include <vector>

#ifdef unix
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#else
#include "psapi.h"
#include "windows.h"
#endif

#include "MixedStack.h"

using std::priority_queue;
using std::unordered_set;
using std::vector;

#define MAXN 30
#define MAXDIAM 60
#define MAXOUTF 12
#define START 10
#define STARTUB 13 // hint for asearch

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int minwr, maxwr;
FILE *outf[MAXOUTF + 1];
long long triescnt; // statistics - number of all tried stacks
long long gencnt;   // number of stacks of size 10 generated so far
int verb;           // be verbose
int globn;          // size of the final stacks (first param. of the program)
int frac;           // try only 1/frac of stacks of size START
int modeq;          // try the modeq-th of frac parts of the stacks
long long fact[MAXN + 2];                     // factorial
long long visstackcnt[MAXN + 2][MAXDIAM + 2]; // data for .scnt file
int diam[MAXN + 2]; // diam[i] = max num of flips among tried stacks of size i

void write_stack(FILE *outf, int vn, int *p) {
  int i;
  for (i = 0; i < vn; i++) {
    fprintf(outf, "%d ", p[i] + 1);
  }
  fprintf(outf, "\n");
}

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

// heap to store current stacks for A* search
// on top is the stack which gives the smallest lower bound for the initial
// stack, if they are more, then take from them one of the furthest from the
// initial stack
class HeapElement {
public:
  HeapElement(MixedStack s_, uint8_t dep_, uint8_t lb_)
      : s(s_), dep(dep_), lb(lb_) {}

  MixedStack s;
  uint8_t dep; // depth ... distance (number of flips) from the initial stack
  uint8_t lb;  // lower bound on the initial stack (i.e., dep is counted, too)
};

// Is el1 greater than el2? This will put the stack with smallest lb to the top
// of the priority_queue.
bool compare_heap_elements(HeapElement el1, HeapElement el2) {
  int tmp = el1.lb - el2.lb;
  if (tmp > 0)
    return true;
  else if (tmp < 0)
    return false;
  else
    return (el2.dep > el1.dep);
}

typedef priority_queue<HeapElement, std::vector<HeapElement>,
                       std::function<bool(HeapElement, HeapElement)>>
    HeapType;

HeapType heap(compare_heap_elements);

size_t maxheapsize = 0;

// flip pancakes 0..fl
void flip_in_place(int *p, int fl) {
  int i, tmp;

  for (i = 0; i < ((fl + 1) >> 1); i++) {
    tmp = p[fl - i];
    p[fl - i] = p[i];
    p[i] = tmp;
  }
}
// flip pancakes 0..fl, save result to np
void flip(int len, int *p, int *np, int fl) {
  int i;

  for (i = 0; i <= fl; i++)
    np[i] = p[fl - i];
  for (; i < len; i++)
    np[i] = p[i];
}

//      UPPER BOUND - a heuristic

// wastes = number of allowed flips that will not add adjacency
// Try a waste only if s.size() is at most
// a) ubbt_dep
// b) or ubbt_end_dep and no join was possible
// Returns the largest number of adjacencies that can be created -> gives
// an upper bound only if the return value equals s.size()
size_t ubbt_dep, ubbt_end_dep;
size_t upper_bound_bt(MixedStack s, int wastes) {
  bool joined = false;
  size_t found = 0;
  size_t res = 0;
  // Flips that create an adjacency.
  unordered_set<size_t> adj_flips;

  for (size_t i = 1; i <= s.size(); i++) {
    if (s.flip_makes_adjacency(i)) {
      found++;
      joined = true;
      adj_flips.insert(i);
      MixedStack sn = s.flip(i);
      size_t tmp = 1 + upper_bound_bt(sn, wastes);
      res = std::max(res, tmp);
      if (res == s.size())
        return res;
      if (found == 2 || (s.bur(0) && found == 1))
        break;
    }
  }
  // try a waste
  if (wastes > 0 &&
      (s.size() <= ubbt_dep || (s.size() <= ubbt_end_dep && !joined))) {
    for (size_t i = 1; i <= s.size(); i++) {
      if (adj_flips.find(i) == adj_flips.end()) {
        MixedStack sn = s.flip(i);
        size_t tmp = upper_bound_bt(sn, wastes - 1);
        res = std::max(res, tmp);
        if (res == s.size())
          return res;
      }
    }
  }
  return res;
}

//   LOWER BOUND - try if it is possible to sort the stack
//                 by making only joins. If not, lower bound
//                 is 1 larger than the number of adjacencies
//                 to be made in the orig. stack

// Returns the longest sequence of joins that can be performed
size_t greedy_steps(MixedStack s) {
  size_t found = 0;
  size_t res = 0;
  for (size_t i = 1; i <= s.size(); i++) {
    if (s.flip_makes_adjacency(i)) {
      found++;
      MixedStack sn = s.flip(i);
      size_t tmp = 1 + greedy_steps(sn);
      res = std::max(res, tmp);
      if (res == s.size())
        return res;
      if (found == 2 || (s.bur(0) && found == 1))
        break;
    }
  }
  return res;
}

//    A*SEARCH
// asres is the length of the shortest sorting sequence found so far
// we are not interested in stacks sortable with < asreqmin flips
// lbcounts is just a statistics
int asres, asreqmin;
long long lbcounts;

// known_lb is set to a nonnegative number only when this stack was tried
// while counting the lower bound for the previous stack (this avoids
// redundant calculations)
void count_lb_and_add_to_heap(MixedStack s, int dep, int known_lb) {

  if (known_lb >= 0) {
    heap.push(HeapElement(s, dep, known_lb));
    maxheapsize = std::max(heap.size(), maxheapsize);
    return;
  }

  int tmpup = -1;
  size_t tmplow = s.size() + dep;
  if (tmplow >= asres)
    return;

  lbcounts++;

  if (greedy_steps(s) == s.size()) {
    tmpup = tmplow;
  } else {
    tmplow++;
    if (tmplow >= asres)
      return;
  }

  if (tmpup == -1) {
    ubbt_dep = 4;
    ubbt_end_dep = 6;
    if (upper_bound_bt(s, 1) == s.size()) {
      tmpup = s.size() + dep + 1;
    }
  }

  if (tmplow >= asres)
    return;
  if (tmpup >= 0) {
    if (tmpup < asres) { // we just found the shortest sorting so far
      asres = tmpup;
      if (asres < asreqmin)
        return;
    }
    if (tmpup == tmplow) {
      return;
    }
  }
  // add to heap
  heap.push(HeapElement(s, dep, tmplow));
  maxheapsize = std::max(heap.size(), maxheapsize);
}

void asearch() {
  while (!heap.empty()) {
    HeapElement el = heap.top();
    heap.pop();
    if (el.lb >= asres) {
      return;
    }
    if (el.s.size() == 0) {
      assert(el.dep < asres);
      asres = el.dep;
      return;
    }

    for (int i = 1; i <= el.s.size(); i++) // flip pancakes 0...i
    {
      MixedStack sn = el.s.flip(i);
      // if we created a new adjacency, it will not be necessary to count lower
      // bound
      int known_lb = (sn.size() < el.s.size()) ? el.lb : -1;
      count_lb_and_add_to_heap(sn, el.dep + 1, known_lb);
      if (asres < asreqmin) {
        return;
      }
    }
  }
}

// start the A*search
int count_dist(int len, int *p, int ub, int reqmin) {
  asres = ub;
  asreqmin = reqmin;

  heap = HeapType(compare_heap_elements);
  vector<uint8_t> orig_stack(len);
  for (int i = 0; i < len; ++i) {
    orig_stack[i] = p[i];
  }
  MixedStack s(orig_stack);
  s.compress();
  MixedStack s2(orig_stack);
  s2.compress_old();
  bool eq = true;
  if (s.size() != s2.size()) {
    eq = false;
  }
  for (size_t i = 0; i < std::min(s.size(), s2.size()); ++i) {
    if (s.val(i) != s2.val(i) || s.bur(i) != s2.bur(i) ||
        s.ori(i) != s2.ori(i)) {
      eq = false;
    }
  }
  if (!eq) {
    std::cerr << s << " differs from" << std::endl << s2 << std::endl;
  }
  // std::cerr << s << std::endl;
  count_lb_and_add_to_heap(s, 0, -1);

  asearch();

  return asres;
}

// Depth-first search through the space of stacks, "depth" is the size -
// starting at START and ending at globn.
// If we know from previous calculations exactly how many flips it needs,
// exact_val is set to this value
void dfs_candidates(int len, int *op, int ub, int exact_val) {
  int i, dst;
  int p[MAXN + 2];
  int np[MAXN + 2];

  triescnt++;
  if (triescnt % 100000 == 0)
    fprintf(stderr,
            "stacks tried %lld, generated %lld of %lld starting stacks\r",
            triescnt, gencnt, fact[START] / frac);

  if (exact_val >= 0)
    dst = exact_val;
  else
    dst = count_dist(len, op, ub, minwr - 2 * (globn - len));
  if (dst + 2 * (globn - len) < minwr)
    return; // the stacks of size globn created from this one would be too easy
            // to sort
  visstackcnt[len][dst]++;
  diam[len] = MAX(diam[len], dst);

  if (len == globn && dst >= minwr && dst <= maxwr) {
    /*if (dst == 11) {
      fprintf(stderr, "FOUND ONE\t\t\t\t\t\t\n");
      write_stack(stderr, len, op);
      vector<uint8_t> st;
      for (int i = 0; i < len; ++i) st.push_back(op[i]);
      std::cerr << MixedStack(st) << std::endl;
    }*/
    if (verb && dst == maxwr) {
      fprintf(stderr, "FOUND ONE\t\t\t\t\t\t\n");
      write_stack(stderr, len, op);
    }
    write_stack(outf[dst - minwr], len, op);
    return;
  }

  for (i = 0; i < len; i++)
    p[i] = op[i];
  // add pancake len to the end and backtrack on the resulting stack
  p[len] = len;
  dfs_candidates(len + 1, p, dst, dst);
  // move pancake len to top and backtrack on the resulting stack
  flip_in_place(p, len);
  dfs_candidates(len + 1, p, dst + 1, -1);
  // try all possible flips and backtrack on the resulting stack
  for (i = 1; i < len; i++) {
    flip(len + 1, p, np, i);
    dfs_candidates(len + 1, np, dst + 2, -1);
  }
}

void process_candidate_start(int len, int *p) {
  gencnt++;
  dfs_candidates(len, p, STARTUB, -1);
}

// generate all stacks of size len and send the ones from modeq'th of frac
// parts to processing
int gcp[MAXN + 2];
int gcused[MAXN + 2];
int gccnt = 0;
void gen_cand(int len, int d) {
  int i, tmp;
  if (d == len) {
    tmp = gccnt;
    gccnt++;
    if (gccnt == frac)
      gccnt = 0;
    if (tmp != modeq)
      return;
    process_candidate_start(len, gcp);
    return;
  }

  if (d == 0)
    for (i = 0; i < len; i++)
      gcused[i] = 0;
  for (i = 0; i < len; i++) {
    if (!gcused[i]) {
      gcp[d] = i;
      gcused[i] = 1;
      gen_cand(len, d + 1);
      gcused[i] = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  int i, j;
  int n;
  char filename[100];
  FILE *scfile;
  FILE *tfile;

  /* MixedStack stack({1, 0, 4, 5, 2, 6, 3});
  std::cerr << stack << std::endl;
  stack = stack.flip(2);
  std::cerr << stack << std::endl;
  stack = stack.flip(3);
  std::cerr << stack << std::endl;
  stack = stack.flip(4);
  std::cerr << stack << std::endl;
  stack = stack.flip(2);
  std::cerr << stack << std::endl;
  stack = stack.flip(1);
  std::cerr << stack << std::endl;
  stack = stack.flip(2);
  std::cerr << stack << std::endl;
  stack = stack.flip(1);
  std::cerr << "size = " << stack.size() << std::endl;
  return 0; */

  // process parameters
  if (argc < 4) {
    std::cerr
        << "Parameters: n minwr maxwr [frac modeq] [v]\n Program determines "
           "stacks of size n with required number of flips between minwr and "
           "maxwr incl. .\n If desired, only 1/frac of stacks are tried; modeq "
           "tells which part among 0...frac-1\n v ... be verbose"
        << std::endl;
    return 0;
  }
  globn = n = atoi(argv[1]);
  if (n > MAXN) {
    std::cerr << "Number of pancakes is too large (must be at most " << MAXN
              << ")." << std::endl;
    return 1;
  }
  if (n < START) {
    std::cerr << "Number of pancakes must be larger than " << START << "."
              << std::endl;
    return 1;
  }
  minwr = atoi(argv[2]);
  maxwr = atoi(argv[3]);
  if (maxwr - minwr > MAXOUTF - 1) {
    std::cerr << "Maxwr-minwr is too large (must be at most " << MAXOUTF - 1
              << ")." << std::endl;
    return 1;
  }
  fact[0] = 1;
  for (i = 1; i <= n; i++)
    fact[i] = fact[i - 1] * i;

  frac = 1;
  modeq = 0;
  if (argc >= 5) {
    if (tolower(argv[4][0]) == 'd' || tolower(argv[4][0]) == 'v')
      verb = 1;
    else
      frac = atoi(argv[4]);
  }
  if (argc >= 6) {
    if (tolower(argv[5][0]) == 'd' || tolower(argv[5][0]) == 'v')
      verb = 1;
    else
      modeq = atoi(argv[5]);
  }
  if (argc >= 7) {
    if (tolower(argv[6][0]) == 'd' || tolower(argv[6][0]) == 'v')
      verb = 1;
  }
  if (frac <= 0) {
    std::cerr << "Frac needs to be a positive integer" << std::endl;
    return 1;
  }
  if (modeq < 0 || modeq >= frac) {
    std::cerr << "Modeq must be among 0, 1, ... frac-1" << std::endl;
    return 1;
  }

  // create files for resulting stacks
  for (i = minwr; i <= maxwr; i++) {
    if (frac == 1)
      sprintf(filename, "%d-%d.in", n, i);
    else
      sprintf(filename, "%d-%d-%04dof%d.in", n, i, modeq, frac);
    if (fopen(filename, "r")) {
      std::cerr << "Output file " << filename << " already exists" << std::endl;
      return 1;
    }
    outf[i - minwr] = fopen(filename, "w");
    assert(outf[i - minwr]);
  }

  // run
  gen_cand(START, 0);

  for (i = minwr; i <= maxwr; i++) {
    fclose(outf[i - minwr]);
  }

  // write the statistics to files .time	and .scnt
  if (frac == 1)
    sprintf(filename, "%d-%d-%d.time", n, minwr, maxwr);
  else
    sprintf(filename, "%d-%d-%d-%04dof%d.time", n, minwr, maxwr, modeq, frac);
  tfile = fopen(filename, "w");
  fprintf(tfile, "manysteps %d %d %d %d %4d ", n, minwr, maxwr, frac, modeq);
  fprintf(tfile, "%10lld %14lld %12lu ", triescnt, lbcounts, maxheapsize);
  fprintf(tfile, "%10.3f\n", my_get_seconds(NULL, NULL));
  fclose(tfile);

  if (frac == 1)
    sprintf(filename, "%d-%d-%d.scnt", n, minwr, maxwr);
  else
    sprintf(filename, "%d-%d-%d-%04dof%d.scnt", n, minwr, maxwr, modeq, frac);
  scfile = fopen(filename, "w");
  fprintf(scfile, "manysteps %d %d %d %d %4d\n", n, minwr, maxwr, frac, modeq);
  fprintf(scfile, "Stack counts:\n");
  for (i = START; i <= n; i++) {
    fprintf(scfile, "n==%02d, diam==%02d ", i, diam[i]);
    for (j = maxwr; j >= MAX(0, minwr - 2 * (n - i)); j--)
      if (visstackcnt[i][j] != 0)
        fprintf(scfile, "%8lld ", visstackcnt[i][j]);
    fprintf(scfile, "\n");
  }
  fclose(scfile);

  return 0;
}
