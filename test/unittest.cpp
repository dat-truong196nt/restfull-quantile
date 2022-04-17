#include <iostream>
#include "../include/helpers.hpp"
#include "../include/handlers.hpp"

using namespace std;

#define DB_TMP "db_tmp"
#define DB_TEST "db_test"

class UnitTest
{
private:
  /* data */
  Database* db;
  /* checking methods */
  bool helpers_check_insertionSort();
  bool helpers_check_quantileCalculate();
  bool database_check_add();
  bool database_check_get_total();
  bool database_check_is_existed();
  bool database_check_get_values();

public:
  void run();
  UnitTest(/* args */);
  ~UnitTest();
};

bool UnitTest::helpers_check_insertionSort()
{
  vector<int64_t> sortingList = {2, 8, -3, 5, 1, -6};
  vector<int64_t> expectedResult = {-6, -3, 1, 2, 5, 8};

  insertionSort(sortingList);
  return sortingList == expectedResult;
}

bool UnitTest::helpers_check_quantileCalculate()
{
  vector<int64_t> values = {-6, -3, 1, 2, 5, 8};
  float quantile = 58.6;
  int64_t expectedResult = 1;

  return expectedResult == quantileCalculate(values, quantile);
}

bool UnitTest::database_check_get_total()
{
  int64_t existedPoolId = 456;
  int64_t invalidPoolId = 257;
  int expectedResult = 4;

  return expectedResult == db->get_total(existedPoolId) && 0 == db->get_total(invalidPoolId);
}

bool UnitTest::database_check_is_existed()
{
  int64_t existedPoolId = 456;
  int64_t invalidPoolId = 257;

  return true == db->is_existed(existedPoolId) && false == db->is_existed(invalidPoolId);
}

bool UnitTest::database_check_get_values()
{
  int64_t existedPoolId = 456;
  int64_t invalidPoolId = 257;
  vector<int64_t> expectedResult = {4, 7, 89, 110};

  return expectedResult == db->get_values(existedPoolId) && 0 == db->get_values(invalidPoolId).size();
}

bool UnitTest::database_check_add()
{
  int64_t existedPoolId = 456;
  int64_t invalidPoolId = 257;
  vector<int64_t> values = {-6, 7, 2, -8};
  vector<int64_t> expectedResult = {-8, -6, 2, 4, 7, 7, 89, 110};
  db->add(existedPoolId, values);
  db->add(invalidPoolId, values);

  return expectedResult == db->get_values(existedPoolId)  && values == db->get_values(invalidPoolId);
}

void UnitTest::run()
{
  int passed = 0, total = 0;

  total++;
  if (!helpers_check_insertionSort()) {
    cerr << "insertionSort failed" << endl;
  } else {
    passed++;
  }

  total++;
  if (!helpers_check_quantileCalculate()) {
    cerr << "insertionSort failed" << endl;
  } else {
    passed++;
  }

  total++;
  if (!database_check_get_total()) {
    cerr << "insertionSort failed" << endl;
  } else {
    passed++;
  }

  total++;
  if (!database_check_is_existed()) {
    cerr << "insertionSort failed" << endl;
  } else {
    passed++;
  }

  total++;
  if (!database_check_add()) {
    cerr << "insertionSort failed" << endl;
  } else {
    passed++;
  }

  cout << "Passed " << passed << " on " << total << " tests." << endl;
}

UnitTest::UnitTest(/* args */)
{
  stringstream cmd;
  cmd << "rm -f " << DB_TEST;
  system(cmd.str().c_str());
  cmd.clear();
  cmd << "cp " << DB_TMP << " " << DB_TEST;
  system(cmd.str().c_str());
  db = new Database(DB_TEST);
}

UnitTest::~UnitTest()
{
  delete db;
}



int main() {
  UnitTest test;
  test.run();
  return 0;
}