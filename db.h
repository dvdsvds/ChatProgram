#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <iostream>

class db {
private:
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;
public:
    db();
    ~db();

    bool connect();
    void close();
    MYSQL* getConnection();

    bool checkUsername(const std::string& username); 

};

void logError(const std::string& err);
void logInfo(const std::string& info);
#endif
