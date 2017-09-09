//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef PREPAREDSTATEMENT_H
#define PREPAREDSTATEMENT_H


#include<stddef.h>

#include<string>
#include<tuple>

#include<sqlite3.h>

#include"QueryResultSet.h"

class SqliteDB;
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;


class PreparedStatement
{
private:
    friend class SqliteDB;
    //只能由SqliteDB类构造PreparedStatement
    PreparedStatement(sqlite3 *db, const std::string &sql);

public:
    PreparedStatement(const PreparedStatement& )=delete;
    PreparedStatement& operator = (const PreparedStatement& )=delete;
    ~PreparedStatement();

    int close();


    int update();
    template<typename... Args>
    int update(Args && ... args);

    int reset();

    std::tuple<int, QueryResultRowSet, QueryResultRowSet> query();
    template<typename ... Args>
    std::tuple<int, QueryResultRowSet, QueryResultRowSet> query(Args && ... args);


    const char* errorMsg();

private:
    template<typename T, typename... Args>
    int bindParams(int param_index, T &&first, Args && ...args);

    template<typename T>
    int bindParams(int param_index, T &&first);


    int bindValue(int param_index, int val);
    int bindValue(int param_index, int64_t val);
    int bindValue(int param_index, double val);
    int bindValue(int param_index, const std::string &val);

private:
    sqlite3_stmt *m_stmt;
};


template<typename... Args>
int PreparedStatement::update(Args && ... args)
{
    int ret = SQLITE_OK;

    do
    {
        ret = bindParams(1, std::forward<Args>(args)...);
        if( ret != SQLITE_OK )
            break;

        ret = sqlite3_step(m_stmt);

    }while(0);

    return ret;
}


inline int PreparedStatement::update()
{
    return sqlite3_step(m_stmt);
}

template<typename T, typename ... Args>
int PreparedStatement::bindParams(int param_index, T &&first, Args && ...args)
{
    int ret = SQLITE_OK;

    do
    {
        ret = bindValue(param_index, first);
        if( ret != SQLITE_OK )
            break;

        ret = bindParams(param_index+1, std::forward<Args>(args)...);

    }while(0);

    return ret;
}


template<typename T>
int PreparedStatement::bindParams(int param_index, T &&first)
{
    int ret = bindValue(param_index, std::forward<T>(first));
    return ret;
}


template<typename ... Args>
inline std::tuple<int, QueryResultRowSet, QueryResultRowSet> PreparedStatement::query(Args && ... args)
{
    int ret = bindParams(1, std::forward<Args>(args)...);
    return std::make_tuple(ret, QueryResultRowSet(m_stmt, false), QueryResultRowSet(m_stmt, true));
}



inline std::tuple<int, QueryResultRowSet, QueryResultRowSet> PreparedStatement::query()
{
    return std::make_tuple(SQLITE_OK, QueryResultRowSet(m_stmt, false), QueryResultRowSet(m_stmt, true));
}



#endif // PREPAREDSTATEMENT_H

