//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license


#include"QueryResultSet.h"


#include<sqlite3.h>


QueryResultColumn::QueryResultColumn(sqlite3_stmt *stmt, int column_num)
    : m_stmt(stmt),
      m_column_num(column_num)
{
}



int QueryResultColumn::getColumnValue(int col, int )
{
    return sqlite3_column_int(m_stmt, col);
}

double QueryResultColumn::getColumnValue(int col, double)
{
    return sqlite3_column_double(m_stmt, col);
}


std::string QueryResultColumn::getColumnValue(int col, const std::string&)
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



QueryResultRowSet::const_reference QueryResultRowSet::operator *()
{
    return QueryResultColumn(m_stmt, m_column_num);
}


QueryResultRowSet::const_pointer QueryResultRowSet::operator -> ()
{
    return &(operator*());
}


QueryResultRowSet::self& QueryResultRowSet::operator ++ ()
{
    nextRow();

    return *this;
}



void QueryResultRowSet::nextRow()
{
    if( m_row_index == -1 )
        return ;

    int ret = sqlite3_step(m_stmt);
    if( ret == SQLITE_ROW )//获得新的一行
        ++m_row_index;
    else if( ret == SQLITE_DONE )//已经取得所有的行
        m_row_index = -1;
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
