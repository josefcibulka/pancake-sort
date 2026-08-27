/*
    Pancake sorting program (see kam.mff.cuni.cz/~cibulka/pancakes)
    Copyright (C) 2009-2026  Josef Cibulka
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
 */

#include <assert.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <vector>

#include "BucketedQueue.h"
#include "Stack.h"
#include "StackLog.h"
#include "constants.h"

using std::vector;

#define START 10
#define STARTUB 13 // hint for asearch

int minwr, maxwr;
long long triescnt; // statistics - number of all tried stacks
long long gencnt;   // number of stacks of size START generated so far
int globn;          // size of the final stacks (first param. of the program)
int frac;           // try only 1/frac of stacks of size START
int modeq;          // try the modeq-th of frac parts of the stacks
long long fact[MAX_SIZE + 2]; // factorial
StackLog slog;

// Priority queue to store the stacks for A* search
BucketedQueue heap;

uint64_t as_per_fp[3];

HeapElement::FlipsProcessed next_fp_list[3]{HeapElement::FLIPS_JOINS,
                                            HeapElement::FLIPS_NON_NEGATIVE,
                                            HeapElement::FLIPS_NON_NEGATIVE};
int adj_diff_want_list[3]{1, 0, -1};

//    A*SEARCH
// asres is the length of the shortest sorting sequence found so far
int asearch(int asres) {
  while (!heap.empty()) {
    HeapElement el = heap.top_pop();
    Stack s = el.s;
#ifdef DEBUG_PRINT
    std::cout << "Taking from heap: " << s << "racnt: " << (int)el.racnt
              << " lb: " << (int)el.lb << std::endl;
#endif
    if (el.racnt == 0) {
      assert(el.dep < asres);
      return el.dep;
    }

    as_per_fp[el.flips_processed]++;

    if (el.flips_processed == HeapElement::FLIPS_NONE) {
      if (s.val(0) > 0) {
        uint8_t pos = s._where_is[s.val(0) - 1];
        if (pos > 1 && s.val(pos - 1) != s.val(0) - 2) {
          Stack ns = s.flip(pos);
          heap.add(HeapElement(ns, el.dep + 1, el.lb, el.racnt - 1));
        }
      }
      uint8_t pos = s._where_is[s.val(0) + 1];
      if (pos > 1 && s.val(pos - 1) != s.val(0) + 2) {
        Stack ns = s.flip(pos);
        if (pos == s.size()) {
          ns.bottom_compress();
        }
        heap.add(HeapElement(ns, el.dep + 1, el.lb, el.racnt - 1));
      }
    } else {
      int adj_diff_want = adj_diff_want_list[el.flips_processed];
      for (size_t i = 2; i <= s.size(); i++) // flip top i pancakes
      {
        int adj_diff = s.adj_diff(i);
        if (adj_diff != adj_diff_want) {
          continue;
        }
        Stack ns = s.flip(i);
        if (i == s.size()) {
          ns.bottom_compress();
        }
        uint8_t ndep = el.dep + 1;
        uint8_t nracnt = el.racnt - adj_diff;
        uint8_t nlow = ndep + nracnt;
        if (nlow < asres) {
          heap.add(HeapElement(ns, ndep, nlow, nracnt));
        }
      }
    }
    if (el.flips_processed != HeapElement::FLIPS_NON_NEGATIVE) {
      uint8_t nlow = el.lb + 1;
      HeapElement::FlipsProcessed next_fp = next_fp_list[el.flips_processed];
      if (nlow < asres) {
#ifdef DEBUG_PRINT
        std::cout << "Adding to heap: " << s << " flips processed " << next_fp
                  << " dep : " << (int)el.dep << std::endl;
#endif
        heap.add(HeapElement(s, el.dep, nlow, el.racnt, next_fp));
      }
    }
  }
  return asres;
}

// start the A*search
int count_dist(Stack s, int ub) {
  heap.reset();

#ifdef DEBUG_PRINT
  static int64_t prev_flip_cnt = 0;
  int64_t flip_diff = Stack::flip_cnt - prev_flip_cnt;
  std::cout << "Flips done: " << flip_diff << std::endl;
  std::cout << "------------------------------------" << std::endl
            << "Trying " << s << std::endl;
  prev_flip_cnt = Stack::flip_cnt;
#endif

  s.bottom_compress();

  uint8_t racnt = s.size() - s.count_adj();
  if (racnt == ub) {
    // Lower bound (remaining adjacencies) is equal to upper bound.
    return ub;
  }
  heap.add(HeapElement(s, 0, racnt, racnt));
  return asearch(ub);
}

// Depth-first search through the space of stacks, "depth" is the size -
// starting at START and ending at globn. If we know from previous calculations
// exactly how many flips it needs, exact_val is set to this value
void dfs_candidates(Stack s, int ub, int exact_val) {
  triescnt++;
  if (triescnt % 100000 == 0)
    fprintf(stderr,
            "stacks tried %lld, generated %lld of %lld starting stacks\r",
            triescnt, gencnt, fact[START] / frac);

  int dst = exact_val;
  if (dst < 0) {
    int reqmin = minwr - 2 * (globn - s.size());
    if (ub < reqmin) {
      // no need for exact value - this stack is too easy to sort
      dst = ub;
    } else {
      dst = count_dist(s, ub);
    }
  }
  if (dst + 2 * (globn - (int)s.size()) < minwr)
    return; // the stacks of size globn created from this one would be too easy
            // to sort
  slog.log_stack(s, dst);

  if ((int)s.size() == globn) {
    return;
  }

  // add pancake len to the end and backtrack on the resulting stack
  Stack extended = s.extend();
  dfs_candidates(extended, dst, dst);
  // move pancake len to top and backtrack on the resulting stack
  Stack ns = extended.flip(extended.size());
  dfs_candidates(ns, dst + 1, -1);
  // try all possible flips and backtrack on the resulting stack
  for (std::size_t i = 2; i < ns.size(); i++) {
    dfs_candidates(ns.flip(i), dst + 2, -1);
  }
}

// generate all stacks of size len and send the ones from modeq'th of frac
// parts to processing
void gen_cand(int len) {
  std::vector<uint8_t> svec;
  for (int i = 0; i < len; ++i) {
    svec.push_back(i);
  }
  // gccnt is the number of generated stacks modulo frac
  for (int gccnt = 0;; gccnt++) {
    if (gccnt == frac) {
      gccnt = 0;
    }
    if (gccnt == modeq) {
      gencnt++;
      dfs_candidates(Stack(svec), STARTUB, -1);
    }
    if (!std::next_permutation(svec.begin(), svec.end())) {
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  int n;

  // process parameters
  if (argc < 4) {
    std::cerr
        << "Parameters: n minwr maxwr [frac modeq]\n Program determines "
           "stacks of size n with required number of flips between minwr and "
           "maxwr incl. .\n If desired, only 1/frac of stacks are tried; modeq "
           "tells which part among 0...frac-1"
        << std::endl;
    return 0;
  }
  globn = n = atoi(argv[1]);
  if (n > MAX_SIZE) {
    std::cerr << "Number of pancakes is too large (must be at most " << MAX_SIZE
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
  fact[0] = 1;
  for (int i = 1; i <= n; i++)
    fact[i] = fact[i - 1] * i;

  frac = 1;
  modeq = 0;
  if (argc >= 5) {
    frac = atoi(argv[4]);
  }
  if (argc >= 6) {
    modeq = atoi(argv[5]);
  }
  if (frac <= 0) {
    std::cerr << "Frac needs to be a positive integer" << std::endl;
    return 1;
  }
  if (modeq < 0 || modeq >= frac) {
    std::cerr << "Modeq must be among 0, 1, ... frac-1" << std::endl;
    return 1;
  }

  if (!slog.init(n, minwr, maxwr, frac, modeq)) {
    return 1;
  }

  // run
  gen_cand(START);

  // write the statistics to files .time	and .scnt
  slog.write_stats(triescnt, Stack::flip_cnt, BucketedQueue::maxheapsize);
  slog.write_stack_counts(START);

  std::cerr << "Asearch stacks per FlipsProcessed:" << std::endl;
  for (int i = 0; i < 3; ++i) {
    std::cerr << as_per_fp[i] << " ";
  }
  std::cerr << std::endl;
  return 0;
}
