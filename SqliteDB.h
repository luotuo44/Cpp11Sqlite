//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef SQLITEDB_H
#define SQLITEDB_H

#include<string>
#include<memory>
#include<list>
#include<vector>
#include<tuple>


typedef struct sqlite3 sqlite3;
class PreparedStatement;

class SqliteDB
{
public:
    explicit SqliteDB(const std::string &db_file);

    SqliteDB(const SqliteDB&)=delete;
    SqliteDB& operator = (const SqliteDB&)=delete;

    ~SqliteDB();

    int close();
    int execute(const std::string &sql);
    const char* errorMsg();

    std::weak_ptr<PreparedStatement> createPreparedStatement(const std::string &sql);


    //对应语句 insert into student(id, name, score) values('001', 'aa', 81),('002', 'bb', 82);
    template<typename ... Args>
    int executemany(const std::string &sql, const std::vector<std::tuple<Args...>> &param);

    //对应语句 insert into student(id, name, score) select '001', 'aa', 81 union all select '002', 'bb', 82;
    template<typename ... Args>
    int executemany2(const std::string &sql, const std::vector<std::tuple<Args...>> &param);


private:
    int closeConn();


private:
    template<typename T>
    static std::string valueToString(const T &val)
    {
        return std::to_string(val);
    }

    static std::string valueToString(const std::string &val)
    {
        return "'" + val + "'";
    }

    template<typename Tuple, std::size_t N>
    struct TuplePrinter
    {
        static std::string print(const Tuple &t)
        {
            std::string ret = TuplePrinter<Tuple, N-1>::print(t);
            ret += ", " + valueToString(std::get<N-1>(t));

            return ret;
        }
    };


    template<typename Tuple>
    struct TuplePrinter<Tuple, 1>
    {
        static std::string print(const Tuple &t)
        {
            return valueToString(std::get<0>(t));
        }
    };


private:
    sqlite3 *m_conn = nullptr;
    std::list<std::shared_ptr<PreparedStatement>> m_stmt_list;
};



template<typename ... Args>
int SqliteDB::executemany(const std::string &sql, const std::vector<std::tuple<Args...>> &param)
{

    std::string sql_param;
    sql_param.reserve(param.size() * 20);

    for(const auto &e : param)
    {
        sql_param += "(" + TuplePrinter<decltype(e), sizeof...(Args)>::print(e) + "),";
    }

    sql_param.pop_back();

    return execute(sql + " values " + sql_param);
}


template<typename ... Args>
int SqliteDB::executemany2(const std::string &sql, const std::vector<std::tuple<Args...>> &param)
{
    std::string sql_param;
    sql_param.reserve(param.size() * 20);

    for(const auto &e : param )
    {
        sql_param += " select " + TuplePrinter<decltype(e), sizeof...(Args)>::print(e) + " union all ";
    }

    auto pos = sql_param.rfind("union all");
    if( pos != std::string::npos )//删除最后一个多余的"union all"
        sql_param.erase(pos);

    return execute(sql + sql_param);
}

#endif // SQLITEDB_H

