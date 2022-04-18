#ifndef __HELPERS__
#define __HELPERS__

#include <vector>
#include <cmath>
#include <string>

using namespace std;

enum status { INSERTED, APPENDED, FAILED };

string getStatusStr(status _status);
void insertionSort(vector<int64_t>& sortingList); // faster for nearly sorted list
int64_t quantileCalculate(const vector<int64_t>& values, const float quantile);

#endif