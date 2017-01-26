//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"QueryResultSet.h"


#include<sqlite3.h>


QueryResultColumn::QueryResultColumn()
    : m_stmt(nullptr),
      m_column_num(0)
{
}

QueryResultColumn::QueryResultColumn(sqlite3_stmt *stmt, int column_num)
    : m_stmt(stmt),
      m_column_num(column_num)
{
    m_column_name_index.reserve(m_column_num);
    for(int col = 0; col < column_num; ++col)
    {
        m_column_name_index.emplace_back(sqlite3_column_name(m_stmt, col));
    }
}


int QueryResultColumn::getColumnValue(int col, int )const
{
    return sqlite3_column_int(m_stmt, col);
}


int64_t QueryResultColumn::getColumnValue(int col, int64_t)const
{
    return sqlite3_column_int64(m_stmt, col);
}


double QueryResultColumn::getColumnValue(int col, double)const
{
    return sqlite3_column_double(m_stmt, col);
}


std::string QueryResultColumn::getColumnValue(int col, const std::string&) const
{
    const char *str = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, col));
    return std::string(str);
}



//===================================================================================


QueryResultRowSet::QueryResultRowSet()
    : m_stmt(nullptr),
      m_row_index(-1),
      m_column_num(0)
{
}


QueryResultRowSet::QueryResultRowSet(sqlite3_stmt *stmt, bool is_end)
    : m_stmt(stmt),
      m_row_index(is_end ? -1 : 0),
      m_column_num(0)
{
    if( !is_end )
    {
        m_column_num = sqlite3_column_count(m_stmt);
        nextRow();
    }
}



QueryResultRowSet::const_reference QueryResultRowSet::operator *() const
{
    return m_current_column;
}


QueryResultRowSet::const_pointer QueryResultRowSet::operator -> () const
{
    return &(operator*());
}


QueryResultRowSet::self& QueryResultRowSet::operator ++ ()
{
    nextRow();

    return *this;
}


QueryResultRowSet::self  QueryResultRowSet::operator ++ (int)
{
    QueryResultRowSet tmp = *this;
    ++(*this);
    return tmp;
}


void QueryResultRowSet::nextRow()
{
    if( m_row_index == -1 )
        return ;

    int ret = sqlite3_step(m_stmt);
    if( ret == SQLITE_ROW )//获得新的一行
    {
        ++m_row_index;
        m_current_column = QueryResultColumn(m_stmt, m_column_num);
    }
    else if( ret == SQLITE_DONE )//已经取得所有的行
    {
        m_row_index = -1;
        m_current_column = QueryResultColumn();
    }
    else //失败
    {
        sqlite3 *link_db = sqlite3_db_handle(m_stmt);
        std::string err_msg = sqlite3_errmsg(link_db);
        throw std::runtime_error("error happen when get next row. Reason: " + err_msg);
    }
}


bool QueryResultRowSet::operator == (const QueryResultRowSet &rs)const
{
    return m_stmt == rs.m_stmt && m_row_index == rs.m_row_index;
}


bool QueryResultRowSet::operator != (const QueryResultRowSet &rs)const
{
    return !(*this == rs);
}
