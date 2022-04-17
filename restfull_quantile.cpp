#include "./crow.h"
#include <string>
#include <vector>

using namespace std;

// fileHandler

namespace fileHandler {
  int delItem(int64_t poolId, string& file)
  {
    stringstream cmd;
    cmd << "sed -i '/" << poolId << "\\//d' " << file;
    CROW_LOG_INFO << "delItem: " << cmd.str();
    return system(cmd.str().c_str());
  }

  int addNewItem(pair<int64_t, vector<int64_t>>& item, string& file)
  {
    stringstream cmd;

    cmd << "echo \"" << item.first << "/";
    for (auto value : item.second) {
      cmd << " " << value;
    }
    cmd << "\" >> " << file;
    CROW_LOG_INFO << "addNewItem: " << cmd.str();

    return system(cmd.str().c_str());
  }

  void addItem(pthread_mutex_t *lock, pair<int64_t, vector<int64_t>> item, string file)
  {
    CROW_LOG_INFO << "lock";
    pthread_mutex_lock(lock);
    CROW_LOG_INFO << "delItem";

    delItem(item.first, file);
    CROW_LOG_INFO << "addItem";
    addNewItem(item, file);
    CROW_LOG_INFO << "unlock";

    pthread_mutex_unlock(lock);
  }
}

// end of fileHandler

// dbHandler
#include <fstream>
#include <unordered_map>
#include <vector>
#include <thread>

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
  int addNewItemToFile(int64_t poolId)
  void addItemToFile(int64_t poolId)

public:
  dbHandler(string file);
  ~dbHandler();

  void addItem(const int64_t poolId, const vector<int64_t>& values);
  int getTotal(int64_t poolId) { return poolDb[poolId].size(); }
  vector<int64_t> getValues(int64_t poolId) { return poolDb[poolId]; }
  bool isItemExisted(int64_t poolId) { return poolDb.find(poolId) != poolDb.end(); }
  auto getThis() { return this; }
};

int dbHandler::delItemFromFile(int64_t poolId)
  {
    stringstream cmd;
    cmd << "sed -i '/" << poolId << "\\//d' " << fileName;
    CROW_LOG_INFO << "delItem: " << cmd.str();
    return system(cmd.str().c_str());
  }

  int dbHandler::addNewItemToFile(int64_t poolId)
  {
    stringstream cmd;

    cmd << "echo \"" << item.first << "/";
    for (auto value : poolDb[poolId]) {
      cmd << " " << value;
    }
    cmd << "\" >> " << fileName;
    CROW_LOG_INFO << "addNewItem: " << cmd.str();

    return system(cmd.str().c_str());
  }

  void dbHandler::addItemToFile(int64_t poolId)
  {
    pthread_mutex_lock(lock);

    delItem(poolId);
    addNewItem(poolId);

    pthread_mutex_unlock(lock);
  }

void dbHandler::addItem(const int64_t poolId, const vector<int64_t>& values)
{
  if (isItemExisted(poolId)) {
    poolDb[poolId].insert(poolDb[poolId].end(), values.begin(), values.end());
  } else {
    poolDb.emplace(make_pair(poolId, values));
  }
  sort(poolDb[poolId].begin(), poolDb[poolId].end());

  thread _fileThread(&dbHandler::addItemToFile, this, poolId);
  _fileThread.detach();
}

void dbHandler::parsingLineToDb(string& line)
{
  string buff;
  stringstream ss(line);
  int64_t poolId;
  vector<int64_t> values;

  ss >> buff;
  poolId = static_cast<int64_t>(stoul(buff));
  while (ss >> buff) {
    values.push_back(stoul(buff));
  }

  poolDb.emplace(make_pair(poolId, values));
}

void dbHandler::printDb()
{
  stringstream ss;
  ss << endl;
  for (auto pair : poolDb) {
    ss << pair.first << ":";
    for (auto value : pair.second) {
      ss << " " << value;
    }
    ss << endl;
  }
  CROW_LOG_DEBUG << "Database imported: " << ss.str();
}

void dbHandler::importData()
{
  string buff;
  fstream file;

  file.open(fileName, fstream::in);
  if (!file) {
    CROW_LOG_DEBUG << "Creating db file";
    file.open(fileName, fstream::out);
    if (!file) {
      CROW_LOG_ERROR << "Failed to create db file";
      return;
    }
  } else {
    while (true) {
      getline(file, buff);
      if (!buff.empty()) parsingLineToDb(buff);
      if (file.eof())
        break;
    }
  }

  file.close();
}

dbHandler::dbHandler(string file = DB_NAME)
{
  fileName = file;
  if (pthread_mutex_init(&threadFileLock, NULL) != 0) {
      CROW_LOG_ERROR << "mutex init has failed";
  }
  importData();
  printDb();
}

dbHandler::~dbHandler() {}

// end of dbHandler

// helpers

void insertionSort(vector<int64_t>& sortingList); // faster for nearly sorted list
int64_t quantileCalculate(const vector<int64_t>& values, const float quantile)
{
  int quantileIndex = round(quantile * (values.size() + 1)) - 1;

  if (quantileIndex > values.size() - 1) return *(values.end() - 1);
  else if (quantileIndex < 0) return *values.begin();
  else return values[quantileIndex];
}

// end of helpers


#define PORT_NUMBER 3000

vector<string> msgs;
vector<pair<crow::response *, decltype(chrono::steady_clock::now())>> ress;

int main()
{
  crow::SimpleApp app;
  crow::mustache::set_base(".");

  dbHandler pool;

  CROW_ROUTE(app, "/api/pool")
    .methods("GET"_method)
    ([&](const crow::request &req) {
      crow::json::wvalue jResp;
      auto jBody = crow::json::load(req.body);

      if (jBody.error() ||
        !jBody.has("poolId") ||
        !jBody.has("percentile") ||
        crow::json::get_type_str(jBody["poolId"].t()) != "Number" ||
        crow::json::get_type_str(jBody["percentile"].t()) != "Number")
      {
        jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"percentile\": number(percentage)}";
        return crow::response(400, jResp);
      }

      int64_t poolId = jBody["poolId"].i();
      double percentile = jBody["percentile"].d();

      if (pool.isItemExisted(poolId)) {
        jResp["total"] = pool.getTotal(poolId);
        jResp["quantile"] = quantileCalculate(pool.getValues(poolId), percentile / 100);
      } else {
        jResp["error"] = "poolId has not found. Try to create one through POST(/api/pool)";
        return crow::response(410, jResp);
      }
      return crow::response(jResp);
    });


  CROW_ROUTE(app, "/api/pool")
    .methods("POST"_method)
    ([&](const crow::request &req) {
      crow::json::wvalue jResp;
      auto jBody = crow::json::load(req.body);

      if (jBody.error() ||
        !jBody.has("poolId") ||
        !jBody.has("poolValues") ||
        crow::json::get_type_str(jBody["poolId"].t()) != "Number" ||
        crow::json::get_type_str(jBody["poolValues"].t()) != "List")
      {
        jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"poolValues\": [num1, num2, ...]}";
        return crow::response(400, jResp);
      } else {
        for (auto value : jBody["poolValues"]) {
          if (crow::json::get_type_str(value.t()) != "Number") {
            jResp["error"] = "Wrong json format received. Need {\"poolId\": number, \"poolValues\": [num1, num2, ...]}";
            return crow::response(400, jResp);
          }
        }
      }

      int64_t poolId = jBody["poolId"].i();
      vector<int64_t> values;

      for (auto value : jBody["poolValues"]) {
        values.push_back(value.i());
      }
      if (pool.isItemExisted(poolId)) {
        jResp["status"] = "appendded";
      } else {
        jResp["status"] = "inserted";
      }
      pool.addItem(poolId, values);
      return crow::response(jResp);
    });

  app.port(PORT_NUMBER)
      .multithreaded()
      .run();
}
