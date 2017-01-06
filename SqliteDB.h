//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef SQLITEDB_H
#define SQLITEDB_H

#include<string>

typedef struct sqlite3 sqlite3;


class SqliteDB
{
public:
    SqliteDB(const std::string &db_file);

    SqliteDB(const SqliteDB&)=delete;
    SqliteDB& operator = (const SqliteDB&)=delete;

    ~SqliteDB();

    int close();


    int execute(const std::string &sql);

    const char* errorMsg();

private:
    int closeConn();

private:
    sqlite3 *m_conn = nullptr;
};



#endif // SQLITEDB_H

