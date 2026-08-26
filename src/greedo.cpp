/* See e.g. wikipedia for the definition of pancake sorting.
 * See http://arxiv.org/abs/0901.3119 for more details on the algorithm
 * including references to the sources of its main ideas.
 *
 * Finds all burnt stacks of a given size globn that have no adjacency
 * and can be sorted by a sequence of greedy steps, that is, steps each
 * of which is making a new adjacency.
 */

#include <assert.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <vector>

#include "BurntStack.h"
#include "constants.h"

using std::vector;

int n;
vector<BurntStack> greedos;

void dfs(BurntStack s, int dep) {
  if (dep == n) {
    greedos.push_back(s);
    return;
  }
  for (size_t i = 0; i < s.size(); ++i) {
    if (s.is_adj(i)) {
      dfs(s.flip(i + 1), dep + 1);
    }
  }
}

int main(int argc, char *argv[]) {
  // process parameters
  if (argc < 2) {
    std::cerr << "Parameters: n\n"
                 "Program determines burnt stacks of size n without any "
                 "adjacency that can be sorted by n flips\n"
                 "That is, all the steps in the created sequence create a new "
                 "adjacency."
              << std::endl;
    return 0;
  }
  n = atoi(argv[1]);
  if (n > MAX_SIZE) {
    std::cerr << "Number of pancakes is too large (must be at most " << MAX_SIZE
              << ")." << std::endl;
    return 1;
  }
  // run
  vector<uint8_t> p;
  for (int i = 0; i < n; ++i) {
    p.push_back(i);
  }
  uint16_t ori = 0;
  BurntStack s{p, ori};
  dfs(s, 0);

  std::cerr << "Greedos found: " << greedos.size() << std::endl;

  std::sort(greedos.begin(), greedos.end());

  int maxseq = 0;
  int curseq = 1;
  for (size_t i = 1; i < greedos.size(); ++i) {
    curseq =
        BurntStack::unburnt_equals(greedos[i], greedos[i - 1]) ? curseq + 1 : 1;
    maxseq = std::max(maxseq, curseq);
  }
  std::cerr << "Longest sequence: " << maxseq << std::endl;

  auto last =
      std::unique(greedos.begin(), greedos.end(), BurntStack::unburnt_equals);
  greedos.erase(last, greedos.end());

  std::cerr << "Greedos found: " << greedos.size() << std::endl;

  /*for (size_t i = 0; i < 20; ++i) {
    std::cerr << greedos[i] << std::endl;
  }*/

  return 0;
}
