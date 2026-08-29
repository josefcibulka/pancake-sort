/* See e.g. wikipedia for the definition of pancake sorting.
 * See http://arxiv.org/abs/0901.3119 for more details on the algorithm
 * including references to the sources of its main ideas.
 *
 * Finds all burnt stacks of a given size globn that have no adjacency
 * and can be sorted by a sequence of greedy steps, that is, steps each
 * of which is making a new adjacency.
 */

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <vector>

#include "BurntStack.h"
#include "Stack.h"
#include "constants.h"

using std::string;
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

// Writes all greedos in a packed binary form.
void write_file(int n) {
  std::ofstream file;
  string filename = std::format("greedos{}.bing", n);
  file.open(filename, std::ios::binary);

  size_t i = 0;
  uint32_t cur_ind = greedos[i].to_unburnt().get_index();
  for (uint32_t j = 0; j < cur_ind; ++j) {
    char val = 0;
    file.write(&val, 1);
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
    file.write(&val, 1);
    uint32_t next_ind = ni < greedos.size()
                            ? greedos[ni].to_unburnt().get_index()
                            : greedos.size();
    for (uint32_t j = cur_ind + 1; j < next_ind; ++j) {
      char val = 0;
      file.write(&val, 1);
    }
    i = ni;
    cur_ind = next_ind;
  }
  // Then, for each greedo, write its panacakes' orientations in 2 bytes.
  for (const BurntStack &greedo : greedos) {
    uint16_t ori = greedo.ori_bitmap();
    file.write(reinterpret_cast<char *>(&ori), 2);
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
  write_file(n);

  int maxseq = 0;
  int curseq = 1;
  for (size_t i = 1; i < greedos.size(); ++i) {
    curseq =
        BurntStack::unburnt_equals(greedos[i], greedos[i - 1]) ? curseq + 1 : 1;
    maxseq = std::max(maxseq, curseq);
  }
  std::cerr << "Longest sequence: " << maxseq << std::endl;

  /* for (size_t i = 0; i < 20; ++i) {
    std::cerr << greedos[i] << std::endl;
  } */

  auto last =
      std::unique(greedos.begin(), greedos.end(), BurntStack::unburnt_equals);
  greedos.erase(last, greedos.end());

  std::cerr << "Unburnt stacks with a greedo burnt stack: " << greedos.size()
            << std::endl;

  return 0;
}
