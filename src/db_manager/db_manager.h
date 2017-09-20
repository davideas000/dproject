#ifndef DPROJECT_DB_MANAGER_DB_MANAGER_H
#define DPROJECT_DB_MANAGER_DB_MANAGER_H

#include <string>
#include <vector>
#include "leveldb/db.h"

namespace dproject {
    class DbManager {

        public:
            explicit DbManager(const std::string& db_name);
            virtual ~DbManager();

            void save(const std::vector< std::pair<std::string, 
                    std::string> > &values);

            std::string get(const std::string &key);
            void print();
        private:
            leveldb::DB *db;
    };
} // namespace dproject

#endif //END DPROJECT_DB_MANAGER_DB_MANAGER_H
