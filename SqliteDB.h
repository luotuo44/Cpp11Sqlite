//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef SQLITEDB_H
#define SQLITEDB_H

#include<string>
#include<memory>
#include<list>


typedef struct sqlite3 sqlite3;
class PreparedStatement;


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


    std::weak_ptr<PreparedStatement> createPreparedStatement(const std::string &sql);

private:
    int closeConn();

private:
    sqlite3 *m_conn = nullptr;
    std::list<std::shared_ptr<PreparedStatement>> m_stmt_list;
};



#endif // SQLITEDB_H

