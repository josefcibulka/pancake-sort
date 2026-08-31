/*
    Pancake sorting program (see kam.mff.cuni.cz/~cibulka/pancakes)
    Copyright (C) 2009-2026  Josef Cibulka
*/

/* See e.g. wikipedia for the definition of pancake sorting.
 * See http://arxiv.org/abs/0901.3119 for more details on the algorithm
 * including references to the sources of its main ideas.
 *
 * Finds all unburnt stacks of a given size finaln with required number of flips
 * from a given interval {minwr, ..., maxwr}.
 * Starts by generating the stacks of size START. From each of them, generates
 * stacks of size finaln in steps. In each step, a stack is enlarged by one by
 * first adding a new largest pancake to the bottom (this stack needs exactly
 * the same number of flips as the original one) and then doing two flips, first
 * of which flips the whole stack. This way all the stacks of size finaln would
 * be generated. The intermediate stacks that would produce stacks of size
 * finaln sortable in < minwr flips are pruned. A*search is used to determine
 * the needed number of flips for a given stack.
 *
 * User can also choose to generate only 1/frac of stacks of size START.
 * Run the program without paramaters to find out how.
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "BucketedQueue.h"
#include "FileFormats.h"
#include "GreedoTools.h"
#include "Stack.h"
#include "StackLog.h"
#include "constants.h"

using std::vector;

#define START 10
#define STARTUB 13 // hint for asearch

int minwr, maxwr;
int finaln; // size of the final stacks (first param. of the program)
int frac;   // try only 1/frac of stacks of size START
int modeq;  // try the modeq-th of frac parts of the stacks
StackLog slog;
std::vector<GreedoSet> greedos;

// Priority queue to store the stacks for A* search
BucketedQueue heap;

// Array indexed by the values of the FlipsProcessed enum, for which it stores
// the next element.
constexpr HeapElement::FlipsProcessed next_fp_list[3]{
    HeapElement::FLIPS_JOINS, HeapElement::FLIPS_NON_NEGATIVE,
    HeapElement::FLIPS_NON_NEGATIVE};
// Diff in adjacencies to look for when processing a heap element with the given
// value of FlipsProcessed. E.g. at FLIPS_NONE, we will process flips adding 1
// adjacency, before we move to FLIPS_JOINS.
constexpr int adj_diff_want_list[3]{1, 0, -1};

int gr_check_cnt[MAX_SIZE];

//    A*SEARCH
// ub is the length of the shortest sorting sequence known so far
int asearch(int ub) {
  while (!heap.empty()) {
    HeapElement el = heap.top_pop();
    Stack s = el.s;
#ifdef DEBUG_PRINT
    std::cout << "Taking from heap: " << s << "racnt: " << (int)el.racnt
              << " lb: " << (int)el.lb << std::endl;
#endif
    if (el.racnt == 0) {
      assert(el.dep < ub);
      return el.dep;
    }

    if (el.racnt <= 12 && !el.known_wastie &&
        el.flips_processed == HeapElement::FLIPS_NONE) {
      gr_check_cnt[el.racnt]++;
      MixedStack ms{el.s};
      ms.compress();
      if (greedos[el.racnt].check_stack(ms)) {
        return el.dep + el.racnt;
      } else {
        // Put the stack back to heap, as a known wastie, so also lb increased.
        uint8_t nlow = el.lb + 1;
        if (nlow < ub) {
          heap.add(
              HeapElement(s, el.dep, nlow, el.racnt, el.flips_processed, true));
        }
        continue;
      }
    }

    if (el.flips_processed == HeapElement::FLIPS_NONE) {
      if (s.val(0) > 0) {
        uint8_t pos = s._where_is[s.val(0) - 1];
        if (pos > 1 && s.val(pos - 1) != s.val(0) - 2) {
          Stack ns = s.flip(pos);
          heap.add(HeapElement(ns, el.dep + 1, el.lb, el.racnt - 1,
                               HeapElement::FLIPS_NONE, el.known_wastie));
        }
      }
      uint8_t pos = s._where_is[s.val(0) + 1];
      if (pos > 1 && s.val(pos - 1) != s.val(0) + 2) {
        Stack ns = s.flip(pos);
        if (pos == s.size()) {
          ns.bottom_compress();
        }
        heap.add(HeapElement(ns, el.dep + 1, el.lb, el.racnt - 1,
                             HeapElement::FLIPS_NONE, el.known_wastie));
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
        if (nlow < ub) {
          heap.add(HeapElement(ns, ndep, nlow, nracnt));
        }
      }
    }
    if (el.flips_processed != HeapElement::FLIPS_NON_NEGATIVE) {
      // Put the stack back to heap, with higher FlipsProcessed.
      uint8_t nlow = el.lb + (el.known_wastie ? 0 : 1);
      if (nlow < ub) {
        HeapElement::FlipsProcessed next_fp = next_fp_list[el.flips_processed];
#ifdef DEBUG_PRINT
        std::cout << "Adding to heap: " << s << " flips processed " << next_fp
                  << " dep : " << (int)el.dep << std::endl;
#endif
        heap.add(HeapElement(s, el.dep, nlow, el.racnt, next_fp, false));
      }
    }
  }
  return ub;
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
// starting at START and ending at finaln. If we know from previous calculations
// exactly how many flips it needs, exact_val is set to this value, otherwise it
// is negative
void dfs_candidates(Stack s, int ub, int exact_val) {
  slog.log_tried_candidate(START);

  int dst = exact_val;
  // If this stack is sortable with less than reqmin steps, then stacks of size
  // finaln generated from it will be sortable with less than minwr steps making
  // them uninteresting.
  int reqmin = minwr - 2 * (finaln - s.size());
  if (ub < reqmin) {
    return;
  }
  if (dst < 0) {
    dst = count_dist(s, ub);
  }
  if (dst < reqmin) {
    return;
  }
  slog.log_stack(s, dst);

  if ((int)s.size() == finaln) {
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
      slog.log_starting_stack();
      dfs_candidates(Stack(svec), STARTUB, -1);
    }
    if (!std::next_permutation(svec.begin(), svec.end())) {
      break;
    }
  }
}

int main(int argc, char *argv[]) {
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
  finaln = atoi(argv[1]);
  if (finaln > MAX_SIZE) {
    std::cerr << "Number of pancakes is too large (must be at most " << MAX_SIZE
              << ")." << std::endl;
    return 1;
  }
  if (finaln < START) {
    std::cerr << "Number of pancakes must be larger than " << START << "."
              << std::endl;
    return 1;
  }
  minwr = atoi(argv[2]);
  maxwr = atoi(argv[3]);
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

  if (!slog.init(finaln, minwr, maxwr, frac, modeq)) {
    return 1;
  }

  GreedosRead reader;
  reader.init(1, 12);
  greedos.push_back(GreedoSet{});
  for (int i = 1; i <= 12; ++i) {
    greedos.push_back(reader.read(i));
  }
  // run
  gen_cand(START);

  // write the statistics to files .time	and .scnt
  slog.write_stats(Stack::flip_cnt, BucketedQueue::maxheapsize,
                   GreedoSet::check_cnt);
  slog.write_stack_counts(START);

  std::cerr << std::endl;
  for (int i = 0; i <= 12; ++i) {
    std::cerr << gr_check_cnt[i] << " ";
  }
  std::cerr << std::endl;

  return 0;
}
