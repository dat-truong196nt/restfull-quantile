#ifndef __DB_HANDLER__
#define __DB_HANDLER__

#include <fstream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <set>
#include "helpers.hpp"
#include "crow.h"

using namespace std;

#define INSTANCES {"pool_0", "pool_1", "pool_2", "pool_3", "pool_4"}
#define MAX_INSTANCE 5
#define DB_ID_VALUES_SPLIT_CHAR "/"
#define MAX_REQUEST 5000 // requests
#define MAX_TIME_OUT 300 // seconds

class Database
{
  private:
    string fileName;
    pthread_mutex_t lock;
    unordered_map<int64_t, vector<int64_t>> poolDb;
    int requestCnt, timeoutCnt;

    void reset_counter();
    int remove(int64_t poolId);
    int append(int64_t poolId);
    void parse_str_to_map(string& line);
    void print();
    void load();

  public:
    set<int64_t> idRequested;

    void run();
    void write(set<int64_t> writeList);
    status add(const int64_t poolId, const vector<int64_t>& values);
    int get_total(int64_t poolId);
    bool is_existed(const int64_t poolId) { return poolDb.find(poolId) != poolDb.end(); }
    vector<int64_t> get_values(const int64_t poolId) { return poolDb[poolId]; }
    Database(const string& file);
    ~Database();
};

class DbHandler
{
private:
  map<int, Database*> instances;
  vector<string> dbList;
  int instanceCnt;
public:
  Database* get_connection(int64_t poolId);
  DbHandler();
  ~DbHandler();
};

#endif