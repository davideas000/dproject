//#include <sqlite/execute.hpp> // ====
#include "db_manager.h"

#include "leveldb/write_batch.h"

#include <sstream>
#include <iostream>


//constructor and destructors
dproject::DbManager::DbManager(const std::string& db_name)
    :db(nullptr)
{
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, db_name, &db);
    assert(status.ok());
}

dproject::DbManager::~DbManager() {
    if (db) {
        delete db;
    }
}

void dproject::DbManager::save(const std::vector< std::pair<std::string, std::string> > &values) {

    //testb
    //#include "leveldb/write_batch.h"
    //...
    //std::string value;
    //leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);
    //if (s.ok()) {
    //leveldb::WriteBatch batch;
    //batch.Delete(key1);
    //batch.Put(key2, value);
    //s = db->Write(leveldb::WriteOptions(), &batch);
    //teste

    leveldb::WriteBatch batch;
    leveldb::Status status;
    for (auto& value : values) {
        batch.Put(value.first, value.second);
    }
    status = db->Write(leveldb::WriteOptions(), &batch);
    assert(status.ok());
}

std::string dproject::DbManager::get(const std::string &key) {
    leveldb::Status status;
    std::string value;
    status = db->Get(leveldb::ReadOptions(), key, &value);
    assert(status.ok());
    return value;
}

void dproject::DbManager::print() {

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::cout << it->key().ToString() << ": " << it->value().ToString() << std::endl;

    }
    assert(it->status().ok());
    delete it;
}
