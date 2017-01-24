//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef QUERYRESULTSET_H
#define QUERYRESULTSET_H


#include<iterator>
#include<string>
#include<stdexcept>
#include<map>


typedef struct sqlite3_stmt sqlite3_stmt;

class QueryResultColumn
{
public:
    QueryResultColumn();
    QueryResultColumn(sqlite3_stmt *stmt, int column_num);

    template<typename T>
    T getColumn(int col)const;

    //根据列名获取，如果查询语句中使用了as别名方式，那么这里使用别名获取
    template<typename T>
    T getColumn(const std::string &column_name)const;

private:

    int getColumnValue(int col, int )const;
    int64_t getColumnValue(int col, int64_t)const;
    double getColumnValue(int col, double)const;
    std::string getColumnValue(int col, const std::string&)const;

private:
    mutable sqlite3_stmt *m_stmt;
    int m_column_num;
    std::map<std::string, int> m_column_name_index;
};



template<typename T>
T QueryResultColumn::getColumn(int col) const
{
    if( col < 0 && col >= m_column_num)
    {
        throw std::out_of_range(std::to_string(col) + " out of range. Column number is " + std::to_string(m_column_num));
    }

    return getColumnValue(col, T());
}


//根据列名获取，如果查询语句中使用了as别名方式，那么这里使用别名获取
template<typename T>
T QueryResultColumn::getColumn(const std::string &column_name)const
{
    auto it = m_column_name_index.find(column_name);
    if( it == m_column_name_index.end() )
    {
        throw std::out_of_range("cannot find column: " + column_name);
    }

    return getColumn<T>(it->second);
}



//==================================================================================



class QueryResultRowSet
{
public:

    using value_type = QueryResultColumn;
    using reference = QueryResultColumn&;
    using const_reference = const QueryResultColumn&;
    using pointer = QueryResultColumn*;
    using const_pointer = const QueryResultColumn*;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;


    using self = QueryResultRowSet;

public:
    QueryResultRowSet();
    QueryResultRowSet(sqlite3_stmt *stmt, bool is_end);


    QueryResultRowSet(const QueryResultRowSet &rs)=default;
    QueryResultRowSet& operator = (const QueryResultRowSet &rs)=default;

    const_reference operator * ()const;
    const_pointer operator -> ()const;

    self& operator ++ ();
    self  operator ++ (int);


    bool operator == (const QueryResultRowSet &rs)const;
    bool operator != (const QueryResultRowSet &rs)const;


private:
    void nextRow();

private:
    sqlite3_stmt *m_stmt;
    int m_row_index;
    int m_column_num;

    QueryResultColumn m_current_column;
};

#endif // QUERYRESULTSET_H

