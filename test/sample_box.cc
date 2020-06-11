/**
 * @file sample_box.cc
 * @brief test
 * @author Ryou Ohsawa
 * @year 2020
 */
#include "minimalKNN.h"
#include <ctime>
#include <random>


int
main(int argn, char** argv)
{
  clock_t t = clock();
  std::mt19937 gen; gen.seed(t);
  std::uniform_real_distribution<double> loc(-10.0, 10.0);

  minimalKNN::vertices V;
  for (size_t i=1; i<1500; i++) {
    const double x = loc(gen);
    const double y = loc(gen);
    const double z = loc(gen);
    V.push_back({x,y,z});
  }

  clock_t t0 = clock();
  minimalKNN::kNNBuilder kNN(V);
  clock_t t1 = clock();
  kNN.print_vertices();
  printf("\n\n");
  kNN.print_nng(1);
  printf("# elapsed time: %lf\n", (double)(t1-t0)/CLOCKS_PER_SEC);
  return 0;
}
