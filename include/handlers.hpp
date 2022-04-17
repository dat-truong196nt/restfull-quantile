#ifndef __DB_HANDLER__
#define __DB_HANDLER__

#include <fstream>
#include <unordered_map>
#include <vector>
#include <thread>
#include "crow.h"

using namespace std;

#define DB_NAME "poolDb"
#define DB_ID_VALUES_SPLIT_CHAR "/"

class dbHandler
{
private:
  /* properties */
  string fileName;
  pthread_mutex_t threadFileLock;
  unordered_map<int64_t, vector<int64_t>> poolDb;

  /* methods */
  void importData();
  void parsingLineToDb(string& line);
  void printDb();
  int delItemFromFile(int64_t poolId);
  int addNewItemToFile(int64_t poolId);
  void addItemToFile(int64_t poolId);

public:
  dbHandler();
  ~dbHandler();

  void addItem(const int64_t poolId, const vector<int64_t>& values);
  int getTotal(int64_t poolId) { return poolDb[poolId].size(); }
  vector<int64_t> getValues(int64_t poolId) { return poolDb[poolId]; }
  bool isItemExisted(int64_t poolId) { return poolDb.find(poolId) != poolDb.end(); }
  auto getThis() { return this; }
};

#endif