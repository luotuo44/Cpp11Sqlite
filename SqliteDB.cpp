//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#include"SqliteDB.h"

#include<stdexcept>

#include<sqlite3.h>


#include"PreparedStatement.h"


SqliteDB::SqliteDB(const std::string &db_file)
    : m_conn(nullptr)
{
    int ret = sqlite3_open(db_file.c_str(), &m_conn);;
    if( ret != SQLITE_OK )
    {
        std::string msg(sqlite3_errmsg(m_conn));
        throw std::runtime_error("fail to open " + db_file + " reason: " + msg);
    }
}

SqliteDB::~SqliteDB()
{
    close();//忽略所有错误
}



int SqliteDB::close()
{
    int ret = closeConn();
    if( ret == SQLITE_OK )
        m_conn = nullptr;

    return ret;
}



int SqliteDB::closeConn()
{
    return sqlite3_close(m_conn);
}



const char* SqliteDB::errorMsg()
{
    return sqlite3_errmsg(m_conn);
}


int SqliteDB::execute(const std::string &sql)
{
    return sqlite3_exec(m_conn, sql.c_str(), nullptr, nullptr, nullptr);
}


//====================================PreparedStatement=====================


std::weak_ptr<PreparedStatement> SqliteDB::createPreparedStatement(const std::string &sql)
{
    std::shared_ptr<PreparedStatement> stmt(new PreparedStatement(m_conn, sql));

    m_stmt_list.push_back(stmt);
    return std::weak_ptr<PreparedStatement>(stmt);
}
