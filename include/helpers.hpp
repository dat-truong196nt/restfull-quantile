#ifndef __HELPERS__
#define __HELPERS__

#include <vector>
#include <cmath>

using namespace std;

void insertionSort(vector<int64_t>& sortingList); // faster for nearly sorted list
int64_t quantileCalculate(const vector<int64_t>& values, const float quantile);

#endif