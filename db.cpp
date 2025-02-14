#include "db.h"
#include <mysql/mysql.h>

void logError(const std::string& err) {
    std::cout << "[ERROR] " << err << std::endl;
}

void logInfo(const std::string& info) {
    std::cout << "[INFO] " << info << std::endl;
}

db::db() {
    conn = mysql_init(nullptr);
}

db::~db() {
    close();
}

bool db::connect() {
    if (!conn) {
        logError("데이터베이스 초기화 실패");
        return false;
    }

    // MySQL 서버 연결 (host, user, password, database, port)
    conn = mysql_real_connect(conn, "211.197.219.43", "yeon", "0735", "Chat", 3306, nullptr, 0);

    if (!conn) {
        logError("데이터베이스  연결 실패");
        logError(mysql_error(conn));
        return false;
    }

    logInfo("데이터베이스 연결 완료");
    return true;
}

void db::close() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
        logInfo("데이터베이스 연결 종료");
    }
}

MYSQL* db::getConnection() {
    return conn;
}

bool db::checkUsername(const std::string& username) {
    conn = mysql_init(nullptr);
    if(!conn) {
        logError("MySQL 초기화 실패");
        return false;
    }
}
