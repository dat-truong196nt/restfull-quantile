#include "../include/handlers.hpp"

Database::Database(const string& file)
{
  fileName = file;
  if (pthread_mutex_init(&lock, NULL) != 0) {
      CROW_LOG_ERROR << "mutex init has failed";
  }
  load();
  reset_counter();
}

Database::~Database() {}

void Database::load()
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
      if (!buff.empty()) parse_str_to_map(buff);
      if (file.eof())
        break;
    }
  }

  file.close();
}

void Database::reset_counter()
{
  requestCnt = 0;
  timeoutCnt = 0;
}

void Database::run()
{
  while (true) {
    sleep(1);
    timeoutCnt++;
    if (requestCnt > MAX_REQUEST || timeoutCnt > MAX_TIME_OUT) {
      thread _wThread(&Database::write, this, set<int64_t>(idRequested.begin(), idRequested.end()));
      _wThread.detach();
      reset_counter();
      idRequested.clear();
    }
  }
}

void Database::parse_str_to_map(string& line)
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

status Database::add(const int64_t poolId, const vector<int64_t>& values)
{
  status _status = status::FAILED;

  if (is_existed(poolId)) {
    poolDb[poolId].insert(poolDb[poolId].end(), values.begin(), values.end());
    _status = status::APPENDED;
  } else {
    poolDb.emplace(make_pair(poolId, values));
    _status = status::INSERTED;
  }
  insertionSort(poolDb[poolId]);
  idRequested.emplace(poolId);
  requestCnt++;

  return _status;
}

int Database::get_total(int64_t poolId)
{
  if (is_existed(poolId))
    return poolDb[poolId].size();
  else
    return 0;
}

void Database::print()
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

int Database::remove(int64_t poolId)
{
  stringstream cmd;
  cmd << "sed -i '/" << poolId << "\\//d' " << fileName;
  CROW_LOG_DEBUG << "delItem: " << cmd.str();
  return system(cmd.str().c_str());
}

int Database::append(int64_t poolId)
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

void Database::write(set<int64_t> writeList)
{
  pthread_mutex_lock(&lock);

  for (auto poolId : writeList) {
    remove(poolId);
    append(poolId);
  }

  pthread_mutex_unlock(&lock);
}

DbHandler::DbHandler()
{
  dbList = vector<string>(INSTANCES);
  instanceCnt = MAX_INSTANCE;
}

DbHandler::~DbHandler() {
  for (auto instance : instances) {
    instance.second->write(instance.second->idRequested);
    delete instance.second;
  }
}

Database* DbHandler::get_connection(int64_t poolId)
{
  const int instanceId = poolId % instanceCnt;

  if (instances.find(instanceId) != instances.end()) {
    return instances[instanceId];
  } else {
    auto *db = new Database(dbList[instanceId]);
    thread(&Database::run, db).detach();
    instances[instanceId] = db;
    return db;
  }
}
