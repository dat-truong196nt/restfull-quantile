#include "../include/handlers.hpp"

dbHandler::dbHandler()
{
  fileName = DB_NAME;
  if (pthread_mutex_init(&threadFileLock, NULL) != 0) {
      CROW_LOG_ERROR << "mutex init has failed";
  }
  importData();
  printDb();
}

dbHandler::~dbHandler() {}

int dbHandler::delItemFromFile(int64_t poolId)
{
  stringstream cmd;
  cmd << "sed -i '/" << poolId << "\\//d' " << fileName;
  CROW_LOG_DEBUG << "delItem: " << cmd.str();
  return system(cmd.str().c_str());
}

int dbHandler::addNewItemToFile(int64_t poolId)
{
  stringstream cmd;

  cmd << "echo \"" << poolId << "/";
  for (auto value : poolDb[poolId]) {
    cmd << " " << value;
  }
  cmd << "\" >> " << fileName;
  CROW_LOG_DEBUG << "addNewItem: " << cmd.str();

  return system(cmd.str().c_str());
}

void dbHandler::addItemToFile(int64_t poolId)
{
  pthread_mutex_lock(&threadFileLock);

  delItemFromFile(poolId);
  addNewItemToFile(poolId);

  pthread_mutex_unlock(&threadFileLock);
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
