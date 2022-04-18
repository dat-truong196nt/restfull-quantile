#include "../include/helpers.hpp"

string getStatusStr(status _status)
{
  switch (_status) {
  case status::APPENDED:
    return "appended";
  case status::INSERTED:
    return "inserted";
  case status::FAILED:
    return "failed";
  default:
    return "not found";
  }
}

static void swap(int& a, int& b)
{
  a = a + b;
  b = a - b;
  a = a - b;
};

void insertionSort(vector<int64_t>& sortingList)
{
  for (int i = 1; i < sortingList.size(); i++) {
      if (sortingList.at(i) > sortingList.at(i - 1))
        continue;
      for (int j = 0; j < i; j++) {
        if (sortingList.at(i) < sortingList.at(j)) {
          sortingList.insert(sortingList.begin() + j, sortingList.at(i));
          sortingList.erase(sortingList.begin() + i + 1);
        }
      }
    }
}

int64_t quantileCalculate(const vector<int64_t>& values, const float quantile)
{
  int quantileIndex = round(quantile * (values.size() + 1)) - 1;

  if (quantileIndex > values.size() - 1) return *(values.end() - 1);
  else if (quantileIndex < 0) return *values.begin();
  else return values[quantileIndex];
}