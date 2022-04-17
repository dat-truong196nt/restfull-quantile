#include "../include/helpers.hpp"

void insertionSort(vector<int64_t>& sortingList); // faster for nearly sorted list
int64_t quantileCalculate(const vector<int64_t>& values, const float quantile)
{
  int quantileIndex = round(quantile * (values.size() + 1)) - 1;

  if (quantileIndex > values.size() - 1) return *(values.end() - 1);
  else if (quantileIndex < 0) return *values.begin();
  else return values[quantileIndex];
}