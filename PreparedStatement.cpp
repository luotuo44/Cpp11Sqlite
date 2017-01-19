//Author: luotuo44@gmail.com
//Use of this source code is governed by a BSD-style license

#include"PreparedStatement.h"

#include<stdexcept>

#include<sqlite3.h>


PreparedStatement::PreparedStatement(sqlite3 *db, const std::string &sql)
    : m_stmt(nullptr)
{
    int ret = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &m_stmt, nullptr);

    if( ret != SQLITE_OK )
    {
        std::string msg(sqlite3_errmsg(db));
        throw std::runtime_error("fail to create PreparedStatement, reason" + msg);
    }
}



PreparedStatement::~PreparedStatement()
{
    close();
}


int PreparedStatement::close()
{
    int ret = SQLITE_OK;
    if( m_stmt != nullptr )
    {
        ret = sqlite3_finalize(m_stmt);

        if( ret == SQLITE_OK )
            m_stmt = nullptr;
    }

    return ret;
}


int PreparedStatement::reset()
{
    return sqlite3_reset(m_stmt);
}

const char* PreparedStatement::errorMsg()
{
    sqlite3 *link_db = sqlite3_db_handle(m_stmt);
    return sqlite3_errmsg(link_db);
}



int PreparedStatement::bindValue(int param_index, int val)
{
    int ret = sqlite3_bind_int(m_stmt, param_index, val);
    return ret;
}


int PreparedStatement::bindValue(int param_index, int64_t val)
{
    int ret = sqlite3_bind_int64(m_stmt, param_index, static_cast<sqlite3_int64>(val));
    return ret;
}


int PreparedStatement::bindValue(int param_index, double val)
{
    int ret = sqlite3_bind_double(m_stmt, param_index, val);
    return ret;
}


int PreparedStatement::bindValue(int param_index, const std::string &val)
{
    int ret = sqlite3_bind_text(m_stmt, param_index, val.c_str(), val.size(), SQLITE_TRANSIENT);
    return ret;
}
