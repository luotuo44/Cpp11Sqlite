//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#ifndef QUERYRESULTSET_H
#define QUERYRESULTSET_H


#include<iterator>
#include<string>
#include<stdexcept>


typedef struct sqlite3_stmt sqlite3_stmt;

class QueryResultColumn
{
public:
    QueryResultColumn(sqlite3_stmt *stmt, int column_num);

    template<typename T>
    T getColumn(int col);

private:

    int getColumnValue(int col, int );
    double getColumnValue(int col, double);
    std::string getColumnValue(int col, const std::string&);

private:
    sqlite3_stmt *m_stmt;
    int m_column_num;
};



template<typename T>
T QueryResultColumn::getColumn(int col)
{
    if( col < 0 && col >= m_column_num)
    {
        throw std::out_of_range(std::to_string(col) + " out of range. Column number is " + std::to_string(m_column_num));
    }

    return getColumnValue(col, T());
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

    const_reference operator * ();
    const_pointer operator -> ();

    self& operator ++ ();


    bool operator == (const QueryResultRowSet &rs)const;
    bool operator != (const QueryResultRowSet &rs)const;


private:
    void nextRow();

private:
    sqlite3_stmt *m_stmt;
    int m_row_index;
    int m_column_num;
};

#endif // QUERYRESULTSET_H

